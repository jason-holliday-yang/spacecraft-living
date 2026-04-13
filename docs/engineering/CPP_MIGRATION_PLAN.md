# C++ Migration Plan

## Goal

Move the project toward a C++-first codebase without changing gameplay rules, controls, save behavior, or content progression.

## Guardrails

- keep public gameplay-facing headers C-compatible
- preserve the current `Game` state layout and runtime flow until each slice is validated
- migrate low-risk infrastructure/content modules before heavy simulation modules
- run smoke tests after each migration slice
- avoid mixing behavior changes with language changes in the same patch

## Target Ownership

### Stay stable during the current migration

- `src/map.c`
- `src/player.c`
- `src/game_play.c`
- `src/task_system.c`

These files currently own hot-path simulation or tightly coupled gameplay rule sets, so they should only move further after the supporting modules have already stabilized.

### Migrate early to C++

- `src/assets.cpp`
- `src/audio_system.cpp`
- `src/localization.cpp`
- `src/recipe_catalog.cpp`
- `src/puzzle.cpp`
- `src/minimap.cpp`
- existing C++ session/render/persistence/platform modules

These modules are lower-risk because they mostly provide content lookup, rendering helpers, optional assets, or orchestration.

## Phases

### Phase 1 - Build And ABI Safety

- keep headers compatible with both C and C++
- make build files treat migrated modules as C++ compilation units
- preserve exported function names and signatures

### Phase 2 - Content And Presentation Helpers

- move static content catalogs and small helper systems to C++
- prefer `std::array`, `static_assert`, and local helper functions for compile-time validation
- keep the callable API in plain C form for the remaining C modules

### Phase 3 - Infrastructure Consolidation

- continue moving asset, audio, save, and tooling helpers to C++
- introduce internal-only C++ helpers where they reduce duplication without changing behavior

### Phase 4 - Gameplay Module Extraction

- split large gameplay modules into smaller files first
- move isolated slices such as recipe checks, objective text, or monster data one subsystem at a time
- only migrate a gameplay slice to C++ after its smoke coverage is in place

## Current Batch

- converted low-risk support modules from `.c` to `.cpp`
- updated `CMakeLists.txt` and `Makefile` so those modules now compile as C++
- kept the public headers and the gameplay-heavy modules unchanged so controls and rules stay intact
- extracted task text tables and initial world content into a dedicated C++ content module while leaving combat/update logic in `src/task_system.c`
- extracted task rendering and read-only query helpers into a dedicated C++ presentation module while keeping the simulation/update loop in `src/task_system.c`
- extracted task interaction, crafting, and attack entry points into a dedicated C++ action module while keeping the underlying runtime state transitions compatible with the existing C gameplay loop
- extracted the log reader, full-map overlay, and save-slot overlay from `src/ui_system.c` into `src/ui_overlays.cpp`
- extracted the remaining UI display responsibilities into focused C++ slices:
  - `src/ui_inventory.cpp`
  - `src/ui_layout.cpp`
  - `src/ui_menu.cpp`
  - `src/ui_status.cpp`
- reduced `src/ui_system.c` to shared drawing/runtime helpers behind `src/ui_runtime_internal.h`
- extracted the runtime simulation update loop from `src/task_system.c` into `src/task_update.c` while keeping the public C API stable

## Next Recommended Batch

1. add a smoke target that exercises menu/session/load flow through the C++ coordinator layer
2. clarify death fallback, rope rules, and Stage 7 route messaging using the new module boundaries
3. decide whether any safe data-driven slices from `src/task_update.c` should move to C++ without touching hot-path state layout
