cmake_minimum_required(VERSION 3.18)
project(SpaceCraftLiving LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RAYLIB_ROOT "/opt/homebrew" CACHE PATH "Prefix path where raylib is installed")

find_path(RAYLIB_INCLUDE_DIR raylib.h
    PATHS
        ${RAYLIB_ROOT}/include
        /opt/homebrew/include
        /usr/local/include
)

find_library(RAYLIB_LIBRARY raylib
    PATHS
        ${RAYLIB_ROOT}/lib
        /opt/homebrew/lib
        /usr/local/lib
)

if (NOT RAYLIB_INCLUDE_DIR OR NOT RAYLIB_LIBRARY)
    message(FATAL_ERROR "raylib was not found. Set RAYLIB_ROOT to the correct installation prefix.")
endif()

set(SPACECRAFT_COMMON_INCLUDE_DIRS
    include
    ${RAYLIB_INCLUDE_DIR}
)

set(SPACECRAFT_COMMON_WARNING_FLAGS
    -Wall
    -Wextra
    -pedantic
)

set(SPACECRAFT_APPLE_FRAMEWORKS
    "-framework IOKit"
    "-framework Cocoa"
    "-framework OpenGL"
    "-framework CoreVideo"
)

set(SPACECRAFT_MAP_SOURCES
    src/map.c
    src/map_layout.c
    src/map_layout_world.c
    src/map_layout_ship.c
    src/map_render_ground.c
    src/map_render_ground_details.c
    src/map_render_ground_ship_forest.c
    src/map_render_ground_swamp_ruins.c
    src/map_render_ground_assets.c
    src/map_render_props.c
    src/map_runtime.c
)

set(SPACECRAFT_PLAYER_SOURCES
    src/player.c
    src/player_status_runtime.c
    src/player_status_text.c
    src/player_consumables.c
    src/player_resources.c
)

set(SPACECRAFT_TASK_RUNTIME_SOURCES
    src/task_seed_setup.c
    src/task_setup.c
    src/task_progress.c
    src/task_economy.c
    src/task_targeting.c
    src/task_survival.c
    src/task_survival_support.c
    src/task_survival_player.c
    src/task_monsters.c
    src/task_update.c
    src/task_system.c
)

set(SPACECRAFT_TASK_CONTENT_SOURCES
    src/recipe_catalog.cpp
    src/task_content.cpp
    src/task_content_data.cpp
    src/task_actions.cpp
    src/task_interactions.cpp
    src/task_ship_interactions.cpp
    src/task_world_interactions.cpp
    src/task_crafting.cpp
    src/puzzle.cpp
)

set(SPACECRAFT_SAVE_SOURCES
    src/save_storage.cpp
    src/save_legacy_runtime.cpp
    src/save_system.cpp
    src/save_buffer_codec.cpp
    src/save_settings_codec.cpp
    src/save_snapshot_codec.cpp
    src/save_snapshot_loader.cpp
    src/save_snapshot_sanitizer.cpp
    src/save_legacy_adapter.cpp
    src/persistence_platform.cpp
)

set(SPACECRAFT_SESSION_SOURCES
    src/game_runtime.cpp
    src/game_save_flow.cpp
    src/game_save_runtime.cpp
    src/game_session.cpp
)

set(SPACECRAFT_UI_LAYOUT_SOURCES
    src/ui_system.c
    src/ui_layout.cpp
)

set(SPACECRAFT_TASK_SMOKE_SOURCES
    ${SPACECRAFT_MAP_SOURCES}
    ${SPACECRAFT_PLAYER_SOURCES}
    ${SPACECRAFT_TASK_RUNTIME_SOURCES}
    ${SPACECRAFT_TASK_CONTENT_SOURCES}
)

set(SPACECRAFT_GAME_SESSION_SMOKE_SOURCES
    ${SPACECRAFT_MAP_SOURCES}
    ${SPACECRAFT_PLAYER_SOURCES}
    ${SPACECRAFT_TASK_RUNTIME_SOURCES}
    ${SPACECRAFT_TASK_CONTENT_SOURCES}
    ${SPACECRAFT_SAVE_SOURCES}
    ${SPACECRAFT_SESSION_SOURCES}
    src/game_play_story.c
    src/audio_system.cpp
    src/localization.cpp
    src/minimap.cpp
    src/task_presentation.cpp
    src/resource_path.cpp
)

set(SPACECRAFT_ENDGAME_FLOW_SOURCES
    ${SPACECRAFT_MAP_SOURCES}
    src/player.c
    src/player_status_runtime.c
    src/player_resources.c
    ${SPACECRAFT_TASK_RUNTIME_SOURCES}
    src/recipe_catalog.cpp
    src/task_actions.cpp
    src/task_interactions.cpp
    src/task_ship_interactions.cpp
    src/task_world_interactions.cpp
    src/task_crafting.cpp
    src/task_content.cpp
    src/task_content_data.cpp
    src/puzzle.cpp
)

set(SPACECRAFT_STATUS_SYSTEM_SOURCES
    ${SPACECRAFT_MAP_SOURCES}
    ${SPACECRAFT_PLAYER_SOURCES}
    src/recipe_catalog.cpp
)

set(SPACECRAFT_SAVE_STATUS_SOURCES
    ${SPACECRAFT_MAP_SOURCES}
    ${SPACECRAFT_SAVE_SOURCES}
)

function(spacecraft_add_executable target)
    add_executable(${target} ${ARGN})
    target_include_directories(${target} PRIVATE ${SPACECRAFT_COMMON_INCLUDE_DIRS})
    target_compile_options(${target} PRIVATE ${SPACECRAFT_COMMON_WARNING_FLAGS})

    if (APPLE)
        target_link_libraries(${target} PRIVATE ${RAYLIB_LIBRARY} ${SPACECRAFT_APPLE_FRAMEWORKS})
    else()
        target_link_libraries(${target} PRIVATE ${RAYLIB_LIBRARY})
    endif()
endfunction()

spacecraft_add_executable(SpaceCraftLiving
    src/main.c
    src/assets.cpp
    src/assets_io.cpp
    src/assets_story_content.cpp
    src/assets_gameplay_content.cpp
    src/audio_system.cpp
    src/resource_path.cpp
    ${SPACECRAFT_SAVE_SOURCES}
    src/localization.cpp
    ${SPACECRAFT_MAP_SOURCES}
    ${SPACECRAFT_PLAYER_SOURCES}
    src/recipe_catalog.cpp
    src/game_session.cpp
    src/game_save_flow.cpp
    src/game_save_runtime.cpp
    src/game_overlay.cpp
    src/game_overlay_frontend.cpp
    src/game_overlay_gameplay.cpp
    src/game_runtime.cpp
    src/game_play.c
    src/game_play_input.c
    src/game_play_story.c
    ${SPACECRAFT_TASK_RUNTIME_SOURCES}
    src/task_actions.cpp
    src/task_interactions.cpp
    src/task_ship_interactions.cpp
    src/task_world_interactions.cpp
    src/task_crafting.cpp
    src/game_render.cpp
    src/task_content.cpp
    src/task_content_data.cpp
    src/task_presentation.cpp
    src/ui_inventory_data.cpp
    src/ui_inventory_icons.cpp
    src/ui_inventory.cpp
    src/ui_backpack.cpp
    src/ui_craft_panel.cpp
    src/ui_layout.cpp
    src/ui_menu.cpp
    src/ui_narrative_panels.cpp
    src/ui_ending_panel.cpp
    src/ui_story_panels.cpp
    src/ui_log_reader.cpp
    src/ui_map_panel.cpp
    src/ui_save_slots_panel.cpp
    src/ui_overlays.cpp
    src/ui_hud.cpp
    src/ui_hud_status.cpp
    src/ui_hud_shortcuts.cpp
    src/ui_info_panels.cpp
    src/ui_status.cpp
    src/ui_system.c
    src/game_manager.cpp
    src/puzzle.cpp
    src/minimap.cpp
)

spacecraft_add_executable(SpaceCraftSaveSmoke
    tests/save_system_smoke.c
    ${SPACECRAFT_SAVE_SOURCES}
    ${SPACECRAFT_MAP_SOURCES}
)

spacecraft_add_executable(SpaceCraftMapSmoke
    tests/map_layout_smoke.c
    ${SPACECRAFT_MAP_SOURCES}
)

spacecraft_add_executable(SpaceCraftTaskRecipeSmoke
    tests/task_recipe_smoke.c
    ${SPACECRAFT_TASK_SMOKE_SOURCES}
)

spacecraft_add_executable(SpaceCraftTaskInteractionSmoke
    tests/task_interaction_smoke.c
    ${SPACECRAFT_TASK_SMOKE_SOURCES}
)

spacecraft_add_executable(SpaceCraftTaskTextSmoke
    tests/task_text_smoke.c
    ${SPACECRAFT_TASK_SMOKE_SOURCES}
)

spacecraft_add_executable(SpaceCraftGameSessionSmoke
    tests/game_session_smoke.c
    ${SPACECRAFT_GAME_SESSION_SMOKE_SOURCES}
)

spacecraft_add_executable(SpaceCraftEndgameFlowSmoke
    tests/endgame_flow_smoke.c
    ${SPACECRAFT_ENDGAME_FLOW_SOURCES}
)

spacecraft_add_executable(SpaceCraftUiLayoutSmoke
    tests/ui_layout_smoke.c
    ${SPACECRAFT_UI_LAYOUT_SOURCES}
)

spacecraft_add_executable(SpaceCraftHealthOxygenSmoke
    tests/health_oxygen_smoke.c
    ${SPACECRAFT_TASK_SMOKE_SOURCES}
)

spacecraft_add_executable(SpaceCraftStatusSystemSmoke
    tests/status_system_smoke.c
    ${SPACECRAFT_STATUS_SYSTEM_SOURCES}
)

spacecraft_add_executable(SpaceCraftCampRecoverySmoke
    tests/camp_recovery_smoke.c
    ${SPACECRAFT_TASK_SMOKE_SOURCES}
)

spacecraft_add_executable(SpaceCraftSaveStatusSmoke
    tests/save_status_smoke.c
    ${SPACECRAFT_SAVE_STATUS_SOURCES}
)

spacecraft_add_executable(SpaceCraftEndgameNewSurvivalSmoke
    tests/endgame_new_survival_smoke.c
    ${SPACECRAFT_TASK_SMOKE_SOURCES}
)

enable_testing()
add_test(NAME save_system_smoke COMMAND SpaceCraftSaveSmoke)
add_test(NAME map_layout_smoke COMMAND SpaceCraftMapSmoke)
add_test(NAME task_recipe_smoke COMMAND SpaceCraftTaskRecipeSmoke)
add_test(NAME task_interaction_smoke COMMAND SpaceCraftTaskInteractionSmoke)
add_test(NAME task_text_smoke COMMAND SpaceCraftTaskTextSmoke)
add_test(NAME game_session_smoke COMMAND SpaceCraftGameSessionSmoke)
add_test(NAME endgame_flow_smoke COMMAND SpaceCraftEndgameFlowSmoke)
add_test(NAME ui_layout_smoke COMMAND SpaceCraftUiLayoutSmoke)
add_test(NAME health_oxygen_smoke COMMAND SpaceCraftHealthOxygenSmoke)
add_test(NAME status_system_smoke COMMAND SpaceCraftStatusSystemSmoke)
add_test(NAME camp_recovery_smoke COMMAND SpaceCraftCampRecoverySmoke)
add_test(NAME save_status_smoke COMMAND SpaceCraftSaveStatusSmoke)
add_test(NAME endgame_new_survival_smoke COMMAND SpaceCraftEndgameNewSurvivalSmoke)
