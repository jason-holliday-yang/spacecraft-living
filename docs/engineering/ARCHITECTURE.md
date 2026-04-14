# SpaceCraft Living Architecture

## Project Shape

The project keeps a flat native game layout:

- `include/`
- `src/`
- `resources/`
- `docs/`

The architecture should continue to protect a few important boundaries:

- simulation state is owned by gameplay systems
- rendering is presentation, not source of truth
- assets are optional and must always have fallbacks
- save data stores gameplay state, not transient render state

The current maintenance/balance iteration also needs to protect two newer constraints:

- world layout must stay legible as `north mainline / east risk / west-south boundary reserve`
- new features should land as smaller modules, not as a return to oversized catch-all files

The current text/localization iteration also needs to protect one more constraint:

- English remains the source text, but localized gameplay copy should keep `English + Simplified Chinese` together in the same definition whenever practical

## Module Responsibilities

### `src/main.c`

- application entry point
- window setup and main loop

### `src/game_manager.cpp`

- top-level game state machine
- frame orchestration, lifetime, and top-level routing
- delegates overlay/gameplay flow to narrower modules

### `src/game_session.cpp`

- new game / opening completion / return-to-menu lifecycle flow
- death threshold handling and recovery-state rebuild

### `src/game_save_flow.cpp`

- save-slot refresh, selection, activation, and deletion flow
- snapshot translation between runtime `Game` state and persistence API
- loaded-save sanitization and safe-tile recovery after world changes

### `src/game_session_internal.h`

- shared internal helpers for session lifecycle and save-flow slices
- transient overlay cleanup, camera reset, gameplay-input reset, and safe-tile lookup

### `src/game_runtime.cpp`

- shared coordinator/runtime helpers used by manager, session, gameplay, and overlay slices
- HUD message lifecycle
- story-scene open/close state transitions
- current gameplay audio-scene selection based on area, stage, and ending state

### `src/game_overlay.cpp`

- overlay state dispatcher
- delegates frontend/session overlays and in-game overlays to narrower slices

### `src/game_overlay_frontend.cpp`

- main menu, opening cutscene, and story-scene overlay input flow
- pause menu, save-panel, settings slider, language switcher, settlement confirm, and ending-screen handling

### `src/localization.cpp`

- runtime language state
- localized UI lookup plus `Loc_PickLiteral` helpers
- `LocalizedText` / co-located EN+ZH authored copy for long-lived data tables
- `Loc_Translate(...)` fallback bridge for legacy runtime strings that still enter the HUD/task-message pipeline as English source text
- translated area/location display names for presentation surfaces
- shared UI-font sample text used to load a Chinese-capable font atlas

### `src/game_overlay_gameplay.cpp`

- crafting, backpack, communicator, help, log-reader, and map overlay input flow
- death-popup input flow and restart/exit dispatch

### `src/game_overlay_internal.h`

- shared overlay input helpers for confirm/navigation/click handling
- internal frontend/gameplay overlay slice declarations

### `src/game_play.c`

- active gameplay frame orchestration during the in-world state
- movement/update/task/audio/death sequencing for the playing state

### `src/game_play_input.c`

- active gameplay input loop during the in-world state
- movement, attack, interaction, crouch-toggle, consumables, and overlay-open routing

### `src/game_play_story.c`

- story-trigger snapshots captured around gameplay updates
- story-scene unlock/open routing for logs, repairs, monoliths, boss defeat, and ending routes

### `src/game_play_internal.h`

- shared internal declarations for gameplay input and story-trigger slices
- keeps the gameplay update entry point stable while allowing the in-world loop to split

### `src/game_render.cpp`

- presentation-only draw order
- player/objective marker rendering helpers
- overlay composition for the current state

### `src/map.c`

- visible-grid traversal and render orchestration
- delegates ground/hazard and prop/camp drawing to narrower render slices

### `src/map_render_ground.c`

- ground tile rendering
- void fallback rendering outside authored biomes
- hazard overlay rendering for poison water, swamp hazards, and similar terrain states

### `src/map_render_props.c`

- prop and landmark rendering
- ship structure, station furniture, crash-site fallback props, trees, and field-camp visuals

### `src/map_layout.c`

- shared map-layout helpers and `Map_Init` orchestration

### `src/map_layout_world.c`

- world terrain and biome footprint placement
- crash-site void perimeter, wilderness props, gates, and world landmarks

### `src/map_layout_ship.c`

- ship interior floorplan seeding
- station and furniture placement
- multi-tile prop bounds for ship furniture and world interactives

### `src/map_runtime.c`

- walkability, opacity, tile lookup, hazard lookup, and area naming
- world-space conversion helpers
- region unlock transitions, field-camp state, and rope-crossing rules

### `src/player.c`

- player initialization, movement, animation timing, and base stat math
- current runtime owns health and oxygen while still carrying legacy stamina / pressure / poison compatibility state used by older rules and save migration
- combat, stealth, and gather-facing runtime modifiers

### `src/player_status_runtime.c`

- status runtime storage, mutation, and decay
- status negativity/priority ordering used by HUD chips
- active-status collection and display ordering

### `src/player_status_text.c`

- player-status name lookup
- status summary/source/relief text
- tooltip composition for HUD and inventory-facing status surfaces

### `src/player_consumables.c`

- quick-consumable logic
- direct backpack item use
- repeat-food diminishing returns and temporary consumable buff application

### `src/player_resources.c`

- resource inventory increments and spending
- player-facing resource labels
- recipe name and summary lookup bridge for UI/task presentation

### `src/task_system.c`

- minimal spatial helper/query functions focused on interaction-distance scoring and blocking actor lookups

### `src/task_setup.c`

- spawn-tile validation, scoring, and placement helpers
- shared monster-spawn helper reused by both init and reinforcement paths

### `src/task_seed_setup.c`

- task runtime initialization
- seed expansion for nodes, monsters, and logs

### `src/task_progress.c`

- objective and communicator refresh
- stage unlock progression helpers
- log reward and monster-drop progression updates
- objective marker, communicator, settlement, and ending accessors

### `src/task_economy.c`

- recipe visibility and craftability checks
- workbench and advanced-crafting requirement validation
- resource spending and gather-yield resolution
- player-facing task message writes shared by task interaction flows

### `src/task_targeting.c`

- nearby node and ship-log lookup
- nearby pickup priority checks
- monster lookup and attack-target selection

### `src/task_update.c`

- thin per-frame task runtime orchestration
- delegates cycle/survival and monster simulation to narrower runtime slices

### `src/task_survival.c`

- day/night cycle and daily event rotation
- node respawn timers
- oxygen, poison, hazard, safe-recovery, and camp-recovery simulation
- low-oxygen, suffocating, critical-condition, filtered, reserve, and leak status derivation

### `src/task_actions.cpp`

- combat entry point for direct player attacks
- attack damage, stamina spend, boss-side modifiers, and kill/reward resolution

### `src/task_interactions.cpp`

- interaction orchestration and preferred-target scoring
- shared recovery helpers used by ship and world interaction slices
- dispatch after node/log pickup priority checks

### `src/task_ship_interactions.cpp`

- ship-console and room interaction logic
- oxygen/airlock/power/terminal handling plus workbench messaging

### `src/task_world_interactions.cpp`

- wilderness interaction logic
- comm relay, crash clue, monolith puzzle, signal tower, camp, and rope handling

### `src/task_crafting.cpp`

- crafting result application after recipe validation
- recipe-specific equipment unlocks, camp placement, and player-facing craft feedback

### `src/task_content.cpp`

- localized stage text and ending text
- progression-facing copy that should stay close to the systems that use it

### `src/task_content_data.cpp`

- authored node seeds, monster seeds, ship-log seeds, log rewards, and event rotation
- ship-log text now keeps English and Simplified Chinese adjacent so future edits stay in one place

### `src/task_presentation.cpp`

- world-space presentation helpers for nodes, monsters, and boss-health visuals
- keeps low-level task-entity drawing details out of higher-level render flow

### `src/ui_system.c`

- shared UI drawing helpers
- wrapped text measurement helpers
- internal runtime bridge exposed through `src/ui_runtime_internal.h`

### `src/ui_layout.cpp`

- all UI rectangle/layout calculators that are part of the public C API

### `src/ui_menu.cpp`

- main menu
- pause menu
- settings overlay presentation
- death popup
- settlement confirmation popup

### `src/ui_narrative_panels.cpp`

- opening cutscene presentation
- story-scene narrative panels
- shared narrative backdrop fallback, texture pan/zoom crop, and lower-third panel layout

### `src/ui_ending_panel.cpp`

- ending screen backdrop, copy, and summary presentation

### `src/ui_story_panels.cpp`

- compatibility translation unit kept as the historical module boundary while narrative and ending panels live in narrower files

### `src/ui_hud.cpp`

- HUD presentation
- lower-left vitals panel and status tooltip flow
- upper-left objective guidance, upper-right mission snapshot, and lower-right quick-access shortcut cards
- transient HUD message and low-health / low-oxygen pulse overlay

### `src/ui_info_panels.cpp`

- communicator overlay presentation
- help overlay presentation

### `src/ui_status.cpp`

- compatibility translation unit kept as the historical module boundary while HUD/info overlays live in narrower files

### `src/minimap.cpp`

- minimap state and exploration update helpers

### `src/recipe_catalog.cpp`

- shared recipe catalog data
- recipe ordering, visibility, and ingredient spending helpers

### `src/puzzle.cpp`

- monolith puzzle rules and hint helpers

### `src/ui_inventory.cpp`

- shared inventory/crafting presentation helpers
- status badge drawing, shared detail-shell composition, and recipe status copy

### `src/ui_inventory_data.cpp`

- backpack entry metadata
- ownership/count helpers
- recipe visual color definitions

### `src/ui_inventory_icons.cpp`

- backpack and recipe icon texture mapping
- fallback icon drawing when assets are missing

### `src/ui_inventory_internal.h`

- shared internal structs and helper declarations for backpack/crafting slices
- keeps the public UI header stable while allowing inventory presentation to split

### `src/ui_backpack.cpp`

- backpack overlay
- item-card state rendering and detail-panel composition

### `src/ui_craft_panel.cpp`

- crafting overlay
- recipe-card state rendering and craft detail-panel composition

### `src/ui_log_reader.cpp`

- log archive overlay

### `src/ui_map_panel.cpp`

- full map overlay
- minimap exploration coloring, prop legend, and objective marker presentation

### `src/ui_save_slots_panel.cpp`

- save-slot overlay
- selected-slot detail panel and delete-action presentation

### `src/ui_overlays.cpp`

- compatibility translation unit kept as the historical overlay-panel boundary while log/map/save overlays live in narrower files

### `src/save_storage.cpp`

- persistence path helpers
- base-directory lookup
- legacy-slot migration
- slot existence queries

### `src/save_system.cpp`

- persistence-facing public API
- `16` slot gameplay snapshot load/save orchestration
- settings file lifecycle and public C ABI boundary

### `src/save_snapshot_loader.cpp`

- snapshot load flow shared by slot listing and save loading
- save-header read, native/legacy dispatch, and post-load sanitization

### `src/save_snapshot_sanitizer.cpp`

- binary snapshot sanity validation
- runtime-safe snapshot normalization before load/save use

### `src/save_buffer_codec.cpp`

- low-level binary reader/writer helpers shared by settings and snapshot codecs
- little-endian integer/float packing
- buffer-bound checks for persistence payload assembly and decoding

### `src/save_settings_codec.cpp`

- binary codec for settings buffers
- current settings encoding and V2 settings decode compatibility

### `src/save_snapshot_codec.cpp`

- binary codec for gameplay save buffers
- current save encoding and V5/V6 native decode compatibility
- isolates byte-level layout details from the higher-level persistence entry points

### `src/save_legacy_runtime.cpp`

- shared compatibility helpers for deriving current `health / status / cleared-door` runtime state from historical save data
- keeps legacy state translation rules reusable between native legacy decode and older binary-layout adapters

### `src/save_legacy_adapter.cpp`

- backward-compatible decoding for legacy save layouts
- isolates historical binary-layout knowledge from the current persistence path

### `src/assets.cpp`

- top-level asset lifecycle orchestration
- player sprite selection, narrative/gameplay asset-batch wiring, and UI font ownership

### `src/assets_io.cpp`

- optional texture and font loading helpers
- sprite-sheet frame analysis and normalization
- fallback-safe texture unload helper

### `src/assets_story_content.cpp`

- intro cutscene, main-story, log-story, and ending art loading
- narrative asset-batch unload flow

### `src/assets_gameplay_content.cpp`

- gameplay/world texture loading for tiles, nodes, icons, and monsters
- gameplay asset-batch unload flow

### `src/audio_system.cpp`

- optional cue and music loading
- scene-based audio switching
- settings application
- safe fallback when referenced music files are missing

### `src/localization.cpp`

- UI-facing text lookup helpers

## Current Architectural Risks

- some design truth still lives in historical docs instead of current docs
- the runtime is still hybrid: player-facing `health / oxygen / status` is live, but several systems still carry legacy `stamina / pressure / poison` compatibility state underneath
- crafting/economy rules and a few content strings still retain pressure-era assumptions that should be cleaned up gradually
- several progression and interaction rules are still implied by code, not documented clearly
- log visibility and progression rules need tighter ownership
- rope traversal behavior is implemented, but its design contract is still vague
- optional asset fallback can hide missing or renamed resources unless the resource docs stay current
- `map.c` is much narrower now, but rendering is still visually dense and lacks dedicated visual regression coverage
- UI modules are much cleaner than before, but future maintenance work must avoid turning communicator/HUD flows into new catch-all coordination points
- save data and survival runtime still need careful cleanup before legacy compatibility fields can be fully retired
- if account/auth capability is ever added later, it must remain optional and isolated from the local single-player runtime

## World Layout Direction

The world-layout direction is now fixed as final product scope:

- ship base sits near the center-top of the world
- ruins / monoliths / boss / signal-tower route live on the north side of the ship
- swamp remains the main risk biome on the east side
- west and south sides stay as intentional boundary reserve / visual whitespace, not planned playable expansion regions

This means map work should avoid sprinkling new landmarks into any free empty corner.
Layout code, objective guidance, and world interaction placement should all reinforce the same directional structure.

The ship interior direction is also fixed:

- one central corridor acts as the ship spine
- cabins branch above and below that corridor
- each cabin must justify its existence with a concrete interactive role

That structure should be reflected consistently across map layout, interaction placement, objective hints, and future art production.

## File Size And Split Rules

Future development must not treat oversized files as an acceptable temporary state.

Required rules:

1. New systems should be added by extending the current slice pattern, not by reopening catch-all files.
2. If a file starts owning more than one major responsibility, split before adding the next feature.
3. Content tables, text, and tuning data should prefer dedicated content modules instead of growing runtime-heavy files.
4. Input flow, runtime simulation, rendering, and persistence should stay in separate modules.
5. A refactor that is known in advance should happen before feature growth, not after the file becomes painful to work in.

Practical limits:

- treat roughly `400-600` lines as the warning zone for a normal feature file
- once a file grows past roughly `700` lines, adding unrelated features to it should be considered a design error unless there is a very strong reason
- large data tables are only acceptable when the file is explicitly a content-table module

Expected split directions for upcoming work:

- ship-cabin interaction growth should not all accumulate inside one interaction file forever
- new map regions should separate layout seeding, runtime hazards, and presentation concerns
- new storyline content should prefer dedicated content tables or chapter-specific helpers
- future account/auth capability should ship as isolated slices, not as branches stuffed into old gameplay or persistence coordinators

## Future Reserve Rules

The only explicitly preserved future capability reserve is optional account/auth support.

Before implementing that capability, the developer should be able to answer:

- which module owns authentication state?
- which module owns remote transport or service access?
- how does the game still boot and play fully offline?
- how does local save ownership remain intact?
- what existing file would become too large if this work were added there directly?

If the answer to the last question is "one of the existing coordinator files", the work should be split first.

## Architectural Priority

The next iteration should improve consistency before expanding feature count:

1. align docs with code truth
2. preserve the fixed world-layout and ship-layout boundaries
3. improve balance, clarity, and release-quality closure without reopening frozen content scope
4. clarify progression-critical interaction rules
5. continue cleaning the hybrid survival/runtime state until legacy pressure-era assumptions are gone
6. keep any future account/auth reserve isolated from the core local gameplay loop
