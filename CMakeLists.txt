cmake_minimum_required(VERSION 3.18)
project(SpaceCraftLiving LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug or Release)" FORCE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB_ROOT "/opt/homebrew" CACHE PATH "Prefix path where raylib is installed")
find_path(RAYLIB_INCLUDE_DIR raylib.h
    PATHS ${RAYLIB_ROOT}/include /opt/homebrew/include /usr/local/include
)
find_library(RAYLIB_LIBRARY raylib
    PATHS ${RAYLIB_ROOT}/lib /opt/homebrew/lib /usr/local/lib
)
if(NOT RAYLIB_INCLUDE_DIR OR NOT RAYLIB_LIBRARY)
    message(FATAL_ERROR "raylib was not found. Set RAYLIB_ROOT to the correct installation prefix.")
endif()

if(APPLE)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    set(SCL_PLATFORM_LIBRARIES
        ${RAYLIB_LIBRARY}
        ${IOKIT_FRAMEWORK}
        ${COCOA_FRAMEWORK}
        ${OPENGL_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
else()
    set(SCL_PLATFORM_LIBRARIES ${RAYLIB_LIBRARY})
endif()

set(SCL_PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SCL_INTERNAL_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app/session"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/content"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gameplay/player"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gameplay/task"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure/assets"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure/audio"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure/save"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/presentation/render"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/presentation/ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/world/map"
)
set(SCL_WARNING_FLAGS -Wall -Wextra -pedantic)

# Keep Tiled data beside out-of-tree executables. ResourcePath_Resolve also
# searches parent directories, so source-tree runs and bundle runs keep the
# same resolution behavior.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/maps" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

set(SCL_ASSET_SOURCES
    src/infrastructure/assets/assets.cpp
    src/infrastructure/assets/assets_gameplay_content.cpp
    src/infrastructure/assets/assets_io.cpp
    src/infrastructure/assets/assets_story_content.cpp
    src/infrastructure/assets/resource_path.cpp
)

set(SCL_AUDIO_SOURCES
    src/infrastructure/audio/audio_system.cpp
)

set(SCL_CONTENT_SOURCES
    src/content/localization.cpp
    src/content/puzzle.cpp
    src/content/recipe_catalog.cpp
    src/content/task_content.cpp
    src/content/task_content_data.cpp
)

set(SCL_MAP_SOURCES
    src/world/map/map_anchors.c
    src/world/map/map_catalog.c
    src/world/map/map_layout.c
    src/world/map/map_layout_ship.c
    src/world/map/map_layout_world.c
    src/world/map/map_prop_bounds.c
    src/world/map/map_regions.c
    src/world/map/map_runtime.c
    src/world/map/map_scene_manager.c
    src/world/map/map_tiled_loader.c
    src/world/map/map_unlocks.c
)

set(SCL_MAP_RENDER_SOURCES
    src/presentation/render/map.c
    src/presentation/render/map_render_decor.c
    src/presentation/render/map_render_ground.c
    src/presentation/render/map_render_ground_assets.c
    src/presentation/render/map_render_ground_details.c
    src/presentation/render/map_render_ground_ship_forest.c
    src/presentation/render/map_render_ground_swamp_ruins.c
    src/presentation/render/map_render_legacy_bounds.c
    src/presentation/render/map_render_props.c
    src/presentation/render/minimap.cpp
)

set(SCL_PLAYER_SOURCES
    src/gameplay/player/player.c
    src/gameplay/player/player_consumables.c
    src/gameplay/player/player_resources.c
    src/gameplay/player/player_status_runtime.c
    src/gameplay/player/player_status_text.c
)

set(SCL_TASK_SOURCES
    src/gameplay/task/task_actions.cpp
    src/gameplay/task/task_archive.c
    src/gameplay/task/task_crafting.cpp
    src/gameplay/task/task_economy.c
    src/gameplay/task/task_ending.c
    src/gameplay/task/task_interactions.cpp
    src/gameplay/task/task_monsters.c
    src/gameplay/task/task_progress.c
    src/gameplay/task/task_runtime_common.c
    src/gameplay/task/task_seed_setup.c
    src/gameplay/task/task_setup.c
    src/gameplay/task/task_ship_interactions.cpp
    src/gameplay/task/task_survival.c
    src/gameplay/task/task_survival_player.c
    src/gameplay/task/task_survival_support.c
    src/gameplay/task/task_system.c
    src/gameplay/task/task_targeting.c
    src/gameplay/task/task_update.c
    src/gameplay/task/task_world_interactions.cpp
)

set(SCL_SAVE_SOURCES
    src/infrastructure/save/persistence_platform.cpp
    src/infrastructure/save/save_buffer_codec.cpp
    src/infrastructure/save/save_legacy_adapter.cpp
    src/infrastructure/save/save_legacy_runtime.cpp
    src/infrastructure/save/save_settings_codec.cpp
    src/infrastructure/save/save_snapshot_codec.cpp
    src/infrastructure/save/save_snapshot_loader.cpp
    src/infrastructure/save/save_snapshot_sanitizer.cpp
    src/infrastructure/save/save_storage.cpp
    src/infrastructure/save/save_system.cpp
)

set(SCL_SESSION_SOURCES
    src/app/session/game_audio_scene.cpp
    src/app/session/game_map_transition.cpp
    src/app/session/game_messages.cpp
    src/app/session/game_route_runtime.cpp
    src/app/session/game_runtime.cpp
    src/app/session/game_save_flow.cpp
    src/app/session/game_save_runtime.cpp
    src/app/session/game_session.cpp
    src/app/session/game_state_sanitize.cpp
    src/app/session/game_transition.cpp
)

set(SCL_UI_SOURCES
    src/presentation/ui/ui_auth.cpp
    src/presentation/ui/ui_backpack.cpp
    src/presentation/ui/ui_components.cpp
    src/presentation/ui/ui_confirm_dialogs.cpp
    src/presentation/ui/ui_craft_panel.cpp
    src/presentation/ui/ui_ending_panel.cpp
    src/presentation/ui/ui_hud.cpp
    src/presentation/ui/ui_hud_shortcuts.cpp
    src/presentation/ui/ui_hud_status.cpp
    src/presentation/ui/ui_info_panels.cpp
    src/presentation/ui/ui_inventory.cpp
    src/presentation/ui/ui_inventory_data.cpp
    src/presentation/ui/ui_inventory_icons.cpp
    src/presentation/ui/ui_layout.cpp
    src/presentation/ui/ui_log_reader.cpp
    src/presentation/ui/ui_map_panel.cpp
    src/presentation/ui/ui_menu.cpp
    src/presentation/ui/ui_narrative_panels.cpp
    src/presentation/ui/ui_overlays.cpp
    src/presentation/ui/ui_save_slots_panel.cpp
    src/presentation/ui/ui_settings.cpp
    src/presentation/ui/ui_status.cpp
    src/presentation/ui/ui_story_panels.cpp
    src/presentation/ui/ui_system.c
    src/presentation/ui/ui_theme.cpp
)

set(SCL_RENDER_SOURCES
    src/presentation/render/game_render.cpp
    src/presentation/render/player_presentation.c
    src/presentation/render/task_presentation.cpp
    src/presentation/render/task_presentation_text.c
)

set(SCL_APP_SOURCES
    src/app/game_manager.cpp
    src/app/game_overlay.cpp
    src/app/game_overlay_auth.cpp
    src/app/game_overlay_backpack.cpp
    src/app/game_overlay_communicator.cpp
    src/app/game_overlay_craft.cpp
    src/app/game_overlay_death.cpp
    src/app/game_overlay_ending.cpp
    src/app/game_overlay_frontend.cpp
    src/app/game_overlay_gameplay.cpp
    src/app/game_overlay_info.cpp
    src/app/game_overlay_main_menu.cpp
    src/app/game_overlay_map.cpp
    src/app/game_overlay_narrative.cpp
    src/app/game_overlay_save.cpp
    src/app/game_overlay_settings.cpp
    src/app/game_play.c
    src/app/game_play_input.c
    src/app/game_play_story.c
)

function(scl_add_object_library target)
    add_library(${target} OBJECT ${ARGN})
    target_include_directories(${target}
        PUBLIC "${SCL_PUBLIC_INCLUDE_DIR}"
        PRIVATE ${SCL_INTERNAL_INCLUDE_DIRS} "${RAYLIB_INCLUDE_DIR}"
    )
    target_compile_options(${target} PRIVATE ${SCL_WARNING_FLAGS})
endfunction()

scl_add_object_library(scl_assets_obj ${SCL_ASSET_SOURCES})
scl_add_object_library(scl_audio_obj ${SCL_AUDIO_SOURCES})
scl_add_object_library(scl_content_obj ${SCL_CONTENT_SOURCES})
scl_add_object_library(scl_map_obj ${SCL_MAP_SOURCES})
scl_add_object_library(scl_map_render_obj ${SCL_MAP_RENDER_SOURCES})
scl_add_object_library(scl_player_obj ${SCL_PLAYER_SOURCES})
scl_add_object_library(scl_task_obj ${SCL_TASK_SOURCES})
scl_add_object_library(scl_save_obj ${SCL_SAVE_SOURCES})
scl_add_object_library(scl_session_obj ${SCL_SESSION_SOURCES})
scl_add_object_library(scl_ui_obj ${SCL_UI_SOURCES})
scl_add_object_library(scl_render_obj ${SCL_RENDER_SOURCES})
scl_add_object_library(scl_app_obj ${SCL_APP_SOURCES})

# One archive is the common internal link boundary for the app and every smoke
# executable. Object libraries compile each module once; tests no longer build
# private copies of the whole source tree.
add_library(spacecraft_internal STATIC
    $<TARGET_OBJECTS:scl_assets_obj>
    $<TARGET_OBJECTS:scl_audio_obj>
    $<TARGET_OBJECTS:scl_content_obj>
    $<TARGET_OBJECTS:scl_map_obj>
    $<TARGET_OBJECTS:scl_map_render_obj>
    $<TARGET_OBJECTS:scl_player_obj>
    $<TARGET_OBJECTS:scl_task_obj>
    $<TARGET_OBJECTS:scl_save_obj>
    $<TARGET_OBJECTS:scl_session_obj>
    $<TARGET_OBJECTS:scl_ui_obj>
    $<TARGET_OBJECTS:scl_render_obj>
    $<TARGET_OBJECTS:scl_app_obj>
)
set_target_properties(spacecraft_internal PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(spacecraft_internal
    PUBLIC "${SCL_PUBLIC_INCLUDE_DIR}" ${SCL_INTERNAL_INCLUDE_DIRS} "${RAYLIB_INCLUDE_DIR}"
)
add_library(SpaceCraft::Internal ALIAS spacecraft_internal)

function(scl_link_internal target)
    target_link_libraries(${target} PRIVATE SpaceCraft::Internal ${SCL_PLATFORM_LIBRARIES})
    target_include_directories(${target} PRIVATE ${SCL_INTERNAL_INCLUDE_DIRS})
    target_compile_options(${target} PRIVATE ${SCL_WARNING_FLAGS})
    set_target_properties(${target} PROPERTIES LINKER_LANGUAGE CXX)
endfunction()

add_executable(SpaceCraftLiving src/app/main.c)
scl_link_internal(SpaceCraftLiving)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_target_properties(SpaceCraftLiving PROPERTIES
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:SpaceCraftLiving>
    DEPENDS SpaceCraftLiving
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    USES_TERMINAL
)

set(SCL_BUNDLE_DIR "${CMAKE_CURRENT_BINARY_DIR}/SpaceCraftLiving.app")
add_custom_target(bundle
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${SCL_BUNDLE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SCL_BUNDLE_DIR}/Contents/MacOS"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SCL_BUNDLE_DIR}/Contents/Resources"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:SpaceCraftLiving>" "${SCL_BUNDLE_DIR}/Contents/MacOS/SpaceCraftLiving"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist" "${SCL_BUNDLE_DIR}/Contents/Info.plist"
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${SCL_BUNDLE_DIR}/Contents/Resources/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/maps" "${SCL_BUNDLE_DIR}/Contents/Resources/maps"
    COMMAND ${CMAKE_COMMAND} -E echo "App bundle created at ${SCL_BUNDLE_DIR}"
    DEPENDS SpaceCraftLiving
    USES_TERMINAL
)

include(CTest)
function(scl_add_smoke_test target test_name source)
    add_executable(${target} "${source}")
    scl_link_internal(${target})
    add_test(NAME ${test_name} COMMAND ${target})
    set_tests_properties(${test_name} PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    string(REGEX REPLACE "_smoke$" "" smoke_short_name "${test_name}")
    add_custom_target(smoke-${smoke_short_name}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG> -R "^${test_name}$"
        DEPENDS ${target}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        USES_TERMINAL
    )
endfunction()

scl_add_smoke_test(SpaceCraftSaveSmoke save_system_smoke tests/save_system_smoke.c)
scl_add_smoke_test(SpaceCraftSaveMultiMapSmoke save_multi_map_smoke tests/save_multi_map_smoke.c)
scl_add_smoke_test(SpaceCraftMapSmoke map_layout_smoke tests/map_layout_smoke.c)
scl_add_smoke_test(SpaceCraftMultiMapSmoke multi_map_smoke tests/multi_map_smoke.c)
scl_add_smoke_test(SpaceCraftMapSceneSmoke map_scene_smoke tests/map_scene_smoke.c)
scl_add_smoke_test(SpaceCraftMapStateSmoke map_state_smoke tests/map_state_smoke.c)
scl_add_smoke_test(SpaceCraftMapPortalSmoke map_portal_smoke tests/map_portal_smoke.c)
scl_add_smoke_test(SpaceCraftMapEntityScopeSmoke map_entity_scope_smoke tests/map_entity_scope_smoke.c)
scl_add_smoke_test(SpaceCraftTaskRecipeSmoke task_recipe_smoke tests/task_recipe_smoke.c)
scl_add_smoke_test(SpaceCraftTaskInteractionSmoke task_interaction_smoke tests/task_interaction_smoke.c)
scl_add_smoke_test(SpaceCraftTaskTextSmoke task_text_smoke tests/task_text_smoke.c)
scl_add_smoke_test(SpaceCraftGameSessionSmoke game_session_smoke tests/game_session_smoke.c)
scl_add_smoke_test(SpaceCraftEndgameFlowSmoke endgame_flow_smoke tests/endgame_flow_smoke.c)
scl_add_smoke_test(SpaceCraftUiLayoutSmoke ui_layout_smoke tests/ui_layout_smoke.c)
scl_add_smoke_test(SpaceCraftHealthOxygenSmoke health_oxygen_smoke tests/health_oxygen_smoke.c)
scl_add_smoke_test(SpaceCraftStatusSystemSmoke status_system_smoke tests/status_system_smoke.c)
scl_add_smoke_test(SpaceCraftCampRecoverySmoke camp_recovery_smoke tests/camp_recovery_smoke.c)
scl_add_smoke_test(SpaceCraftSaveStatusSmoke save_status_smoke tests/save_status_smoke.c)
scl_add_smoke_test(SpaceCraftEndgameNewSurvivalSmoke endgame_new_survival_smoke tests/endgame_new_survival_smoke.c)

add_custom_target(smoke
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>
    DEPENDS
    SpaceCraftSaveSmoke
    SpaceCraftSaveMultiMapSmoke
    SpaceCraftMapSmoke
    SpaceCraftMultiMapSmoke
    SpaceCraftMapSceneSmoke
    SpaceCraftMapStateSmoke
    SpaceCraftMapPortalSmoke
    SpaceCraftMapEntityScopeSmoke
    SpaceCraftTaskRecipeSmoke
    SpaceCraftTaskInteractionSmoke
    SpaceCraftTaskTextSmoke
    SpaceCraftGameSessionSmoke
    SpaceCraftEndgameFlowSmoke
    SpaceCraftUiLayoutSmoke
    SpaceCraftHealthOxygenSmoke
    SpaceCraftStatusSystemSmoke
    SpaceCraftCampRecoverySmoke
    SpaceCraftSaveStatusSmoke
    SpaceCraftEndgameNewSurvivalSmoke
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    USES_TERMINAL
)
