APP_NAME := SpaceCraftLiving
BUILD_DIR := build
RAYLIB_PREFIX ?= /opt/homebrew
.DEFAULT_GOAL := all

CC := clang
CXX := clang++

MAP_C_SOURCES := \
	src/map.c \
	src/map_layout.c \
	src/map_layout_world.c \
	src/map_layout_ship.c \
	src/map_render_ground.c \
	src/map_render_ground_details.c \
	src/map_render_ground_ship_forest.c \
	src/map_render_ground_swamp_ruins.c \
	src/map_render_ground_assets.c \
	src/map_render_props.c \
	src/map_runtime.c

PLAYER_C_SOURCES := \
	src/player.c \
	src/player_status_runtime.c \
	src/player_status_text.c \
	src/player_consumables.c \
	src/player_resources.c

GAMEPLAY_C_SOURCES := \
	src/game_play.c \
	src/game_play_input.c \
	src/game_play_story.c

TASK_RUNTIME_C_SOURCES := \
	src/task_seed_setup.c \
	src/task_setup.c \
	src/task_progress.c \
	src/task_economy.c \
	src/task_targeting.c \
	src/task_survival.c \
	src/task_survival_support.c \
	src/task_survival_player.c \
	src/task_monsters.c \
	src/task_update.c \
	src/task_system.c

UI_RUNTIME_C_SOURCES := \
	src/ui_system.c

TASK_CONTENT_BASE_CXX_SOURCES := \
	src/recipe_catalog.cpp \
	src/task_content.cpp \
	src/task_content_data.cpp \
	src/puzzle.cpp

TASK_ACTION_CXX_SOURCES := \
	src/task_actions.cpp \
	src/task_interactions.cpp \
	src/task_ship_interactions.cpp \
	src/task_world_interactions.cpp \
	src/task_crafting.cpp

TASK_CONTENT_CXX_SOURCES := \
	$(TASK_CONTENT_BASE_CXX_SOURCES) \
	$(TASK_ACTION_CXX_SOURCES)

ASSET_CXX_SOURCES := \
	src/assets.cpp \
	src/assets_io.cpp \
	src/assets_story_content.cpp \
	src/assets_gameplay_content.cpp

SAVE_CXX_SOURCES := \
	src/save_storage.cpp \
	src/save_legacy_runtime.cpp \
	src/save_system.cpp \
	src/save_buffer_codec.cpp \
	src/save_settings_codec.cpp \
	src/save_snapshot_codec.cpp \
	src/save_snapshot_loader.cpp \
	src/save_snapshot_sanitizer.cpp \
	src/save_legacy_adapter.cpp \
	src/persistence_platform.cpp

SESSION_CXX_SOURCES := \
	src/game_runtime.cpp \
	src/game_save_flow.cpp \
	src/game_save_runtime.cpp \
	src/game_session.cpp

OVERLAY_CXX_SOURCES := \
	src/game_overlay.cpp \
	src/game_overlay_frontend.cpp \
	src/game_overlay_gameplay.cpp

UI_CXX_SOURCES := \
	src/ui_inventory_data.cpp \
	src/ui_inventory_icons.cpp \
	src/ui_inventory.cpp \
	src/ui_backpack.cpp \
	src/ui_craft_panel.cpp \
	src/ui_layout.cpp \
	src/ui_menu.cpp \
	src/ui_narrative_panels.cpp \
	src/ui_ending_panel.cpp \
	src/ui_story_panels.cpp \
	src/ui_log_reader.cpp \
	src/ui_map_panel.cpp \
	src/ui_save_slots_panel.cpp \
	src/ui_overlays.cpp \
	src/ui_hud.cpp \
	src/ui_hud_status.cpp \
	src/ui_hud_shortcuts.cpp \
	src/ui_info_panels.cpp \
	src/ui_status.cpp

SUPPORT_CXX_SOURCES := \
	src/audio_system.cpp \
	src/localization.cpp \
	src/minimap.cpp \
	src/resource_path.cpp

LOCALIZATION_CXX_SOURCES := \
	src/localization.cpp

APP_C_SOURCES := \
	src/main.c \
	$(MAP_C_SOURCES) \
	$(PLAYER_C_SOURCES) \
	$(GAMEPLAY_C_SOURCES) \
	$(TASK_RUNTIME_C_SOURCES) \
	$(UI_RUNTIME_C_SOURCES)

APP_CXX_SOURCES := \
	$(ASSET_CXX_SOURCES) \
	$(SUPPORT_CXX_SOURCES) \
	$(TASK_CONTENT_CXX_SOURCES) \
	src/task_presentation.cpp \
	$(SAVE_CXX_SOURCES) \
	$(SESSION_CXX_SOURCES) \
	$(OVERLAY_CXX_SOURCES) \
	src/game_render.cpp \
	$(UI_CXX_SOURCES) \
	src/game_manager.cpp

SAVE_SMOKE_NAME := save_system_smoke
MAP_SMOKE_NAME := map_layout_smoke
TASK_RECIPE_SMOKE_NAME := task_recipe_smoke
TASK_INTERACTION_SMOKE_NAME := task_interaction_smoke
TASK_TEXT_SMOKE_NAME := task_text_smoke
SESSION_SMOKE_NAME := game_session_smoke
ENDGAME_SMOKE_NAME := endgame_flow_smoke
UI_LAYOUT_SMOKE_NAME := ui_layout_smoke
HEALTH_OXYGEN_SMOKE_NAME := health_oxygen_smoke
STATUS_SYSTEM_SMOKE_NAME := status_system_smoke
CAMP_RECOVERY_SMOKE_NAME := camp_recovery_smoke
SAVE_STATUS_SMOKE_NAME := save_status_smoke
ENDGAME_NEW_SURVIVAL_SMOKE_NAME := endgame_new_survival_smoke

TASK_SMOKE_C_SOURCES := \
	$(MAP_C_SOURCES) \
	$(PLAYER_C_SOURCES) \
	$(TASK_RUNTIME_C_SOURCES)

TASK_SMOKE_CXX_SOURCES := \
	$(TASK_CONTENT_CXX_SOURCES) \
	$(LOCALIZATION_CXX_SOURCES)

SAVE_SMOKE_SOURCES := \
	tests/save_system_smoke.c \
	$(MAP_C_SOURCES) \
	$(SAVE_CXX_SOURCES) \
	$(LOCALIZATION_CXX_SOURCES)

MAP_SMOKE_SOURCES := \
	tests/map_layout_smoke.c \
	$(MAP_C_SOURCES)

TASK_RECIPE_SMOKE_SOURCES := \
	tests/task_recipe_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

TASK_INTERACTION_SMOKE_SOURCES := \
	tests/task_interaction_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

TASK_TEXT_SMOKE_SOURCES := \
	tests/task_text_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

SESSION_SMOKE_SOURCES := \
	tests/game_session_smoke.c \
	$(MAP_C_SOURCES) \
	$(PLAYER_C_SOURCES) \
	src/game_play_story.c \
	$(TASK_RUNTIME_C_SOURCES) \
	src/audio_system.cpp \
	src/localization.cpp \
	src/minimap.cpp \
	$(TASK_CONTENT_CXX_SOURCES) \
	src/task_presentation.cpp \
	$(SAVE_CXX_SOURCES) \
	$(SESSION_CXX_SOURCES) \
	src/resource_path.cpp

ENDGAME_SMOKE_SOURCES := \
	tests/endgame_flow_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

UI_LAYOUT_SMOKE_SOURCES := \
	tests/ui_layout_smoke.c \
	$(UI_RUNTIME_C_SOURCES) \
	src/ui_layout.cpp

HEALTH_OXYGEN_SMOKE_SOURCES := \
	tests/health_oxygen_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

STATUS_SYSTEM_SMOKE_SOURCES := \
	tests/status_system_smoke.c \
	$(MAP_C_SOURCES) \
	$(PLAYER_C_SOURCES) \
	src/recipe_catalog.cpp \
	$(LOCALIZATION_CXX_SOURCES)

CAMP_RECOVERY_SMOKE_SOURCES := \
	tests/camp_recovery_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

SAVE_STATUS_SMOKE_SOURCES := \
	tests/save_status_smoke.c \
	$(MAP_C_SOURCES) \
	$(SAVE_CXX_SOURCES) \
	$(LOCALIZATION_CXX_SOURCES)

ENDGAME_NEW_SURVIVAL_SMOKE_SOURCES := \
	tests/endgame_new_survival_smoke.c \
	$(TASK_SMOKE_C_SOURCES) \
	$(TASK_SMOKE_CXX_SOURCES)

CPPFLAGS := -Iinclude -I$(RAYLIB_PREFIX)/include
DEPFLAGS := -MMD -MP
CFLAGS := -std=c11 -Wall -Wextra -pedantic $(DEPFLAGS)
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic $(DEPFLAGS)
LDFLAGS := -L$(RAYLIB_PREFIX)/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo

objects_for = \
	$(patsubst src/%.c,$(BUILD_DIR)/$1/%.o,$(filter src/%.c,$2)) \
	$(patsubst src/%.cpp,$(BUILD_DIR)/$1/%.o,$(filter src/%.cpp,$2)) \
	$(patsubst tests/%.c,$(BUILD_DIR)/$1/tests_%.o,$(filter tests/%.c,$2))

APP_OBJECTS := $(call objects_for,app,$(APP_C_SOURCES) $(APP_CXX_SOURCES))
SAVE_SMOKE_OBJECTS := $(call objects_for,smoke,$(SAVE_SMOKE_SOURCES))
MAP_SMOKE_OBJECTS := $(call objects_for,smoke_map,$(MAP_SMOKE_SOURCES))
TASK_RECIPE_SMOKE_OBJECTS := $(call objects_for,smoke_task,$(TASK_RECIPE_SMOKE_SOURCES))
TASK_INTERACTION_SMOKE_OBJECTS := $(call objects_for,smoke_task_interaction,$(TASK_INTERACTION_SMOKE_SOURCES))
TASK_TEXT_SMOKE_OBJECTS := $(call objects_for,smoke_task_text,$(TASK_TEXT_SMOKE_SOURCES))
SESSION_SMOKE_OBJECTS := $(call objects_for,smoke_session,$(SESSION_SMOKE_SOURCES))
ENDGAME_SMOKE_OBJECTS := $(call objects_for,smoke_endgame,$(ENDGAME_SMOKE_SOURCES))
UI_LAYOUT_SMOKE_OBJECTS := $(call objects_for,smoke_ui,$(UI_LAYOUT_SMOKE_SOURCES))
HEALTH_OXYGEN_SMOKE_OBJECTS := $(call objects_for,smoke_health_oxygen,$(HEALTH_OXYGEN_SMOKE_SOURCES))
STATUS_SYSTEM_SMOKE_OBJECTS := $(call objects_for,smoke_status_system,$(STATUS_SYSTEM_SMOKE_SOURCES))
CAMP_RECOVERY_SMOKE_OBJECTS := $(call objects_for,smoke_camp_recovery,$(CAMP_RECOVERY_SMOKE_SOURCES))
SAVE_STATUS_SMOKE_OBJECTS := $(call objects_for,smoke_save_status,$(SAVE_STATUS_SMOKE_SOURCES))
ENDGAME_NEW_SURVIVAL_SMOKE_OBJECTS := $(call objects_for,smoke_endgame_survival,$(ENDGAME_NEW_SURVIVAL_SMOKE_SOURCES))

ALL_OBJECTS := \
	$(APP_OBJECTS) \
	$(SAVE_SMOKE_OBJECTS) \
	$(MAP_SMOKE_OBJECTS) \
	$(TASK_RECIPE_SMOKE_OBJECTS) \
	$(TASK_INTERACTION_SMOKE_OBJECTS) \
	$(TASK_TEXT_SMOKE_OBJECTS) \
	$(SESSION_SMOKE_OBJECTS) \
	$(ENDGAME_SMOKE_OBJECTS) \
	$(UI_LAYOUT_SMOKE_OBJECTS) \
	$(HEALTH_OXYGEN_SMOKE_OBJECTS) \
	$(STATUS_SYSTEM_SMOKE_OBJECTS) \
	$(CAMP_RECOVERY_SMOKE_OBJECTS) \
	$(SAVE_STATUS_SMOKE_OBJECTS) \
	$(ENDGAME_NEW_SURVIVAL_SMOKE_OBJECTS)

DEPFILES := $(ALL_OBJECTS:.o=.d)

define define_compile_rules
$(BUILD_DIR)/$1/%.o: src/%.c Makefile
	mkdir -p $$(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $$< -o $$@

$(BUILD_DIR)/$1/%.o: src/%.cpp Makefile
	mkdir -p $$(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $$< -o $$@

$(BUILD_DIR)/$1/tests_%.o: tests/%.c Makefile
	mkdir -p $$(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $$< -o $$@
endef

BUILD_SUBDIRS := \
	app \
	smoke \
	smoke_map \
	smoke_task \
	smoke_task_interaction \
	smoke_task_text \
	smoke_session \
	smoke_endgame \
	smoke_ui \
	smoke_health_oxygen \
	smoke_status_system \
	smoke_camp_recovery \
	smoke_save_status \
	smoke_endgame_survival

$(foreach dir,$(BUILD_SUBDIRS),$(eval $(call define_compile_rules,$(dir))))

define define_binary_target
$($1_BIN): $$($1_OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $$($1_OBJECTS) -o $$@ $(LDFLAGS)

.PHONY: $($1_ALIAS)
$($1_ALIAS): $$($1_BIN)
	$$($1_BIN)
endef

SAVE_SMOKE_BIN := $(BUILD_DIR)/$(SAVE_SMOKE_NAME)
SAVE_SMOKE_ALIAS := smoke-save
MAP_SMOKE_BIN := $(BUILD_DIR)/$(MAP_SMOKE_NAME)
MAP_SMOKE_ALIAS := smoke-map
TASK_RECIPE_SMOKE_BIN := $(BUILD_DIR)/$(TASK_RECIPE_SMOKE_NAME)
TASK_RECIPE_SMOKE_ALIAS := smoke-task-recipe
TASK_INTERACTION_SMOKE_BIN := $(BUILD_DIR)/$(TASK_INTERACTION_SMOKE_NAME)
TASK_INTERACTION_SMOKE_ALIAS := smoke-task-interaction
TASK_TEXT_SMOKE_BIN := $(BUILD_DIR)/$(TASK_TEXT_SMOKE_NAME)
TASK_TEXT_SMOKE_ALIAS := smoke-task-text
SESSION_SMOKE_BIN := $(BUILD_DIR)/$(SESSION_SMOKE_NAME)
SESSION_SMOKE_ALIAS := smoke-session
ENDGAME_SMOKE_BIN := $(BUILD_DIR)/$(ENDGAME_SMOKE_NAME)
ENDGAME_SMOKE_ALIAS := smoke-endgame
UI_LAYOUT_SMOKE_BIN := $(BUILD_DIR)/$(UI_LAYOUT_SMOKE_NAME)
UI_LAYOUT_SMOKE_ALIAS := smoke-ui-layout
HEALTH_OXYGEN_SMOKE_BIN := $(BUILD_DIR)/$(HEALTH_OXYGEN_SMOKE_NAME)
HEALTH_OXYGEN_SMOKE_ALIAS := smoke-health-oxygen
STATUS_SYSTEM_SMOKE_BIN := $(BUILD_DIR)/$(STATUS_SYSTEM_SMOKE_NAME)
STATUS_SYSTEM_SMOKE_ALIAS := smoke-status-system
CAMP_RECOVERY_SMOKE_BIN := $(BUILD_DIR)/$(CAMP_RECOVERY_SMOKE_NAME)
CAMP_RECOVERY_SMOKE_ALIAS := smoke-camp-recovery
SAVE_STATUS_SMOKE_BIN := $(BUILD_DIR)/$(SAVE_STATUS_SMOKE_NAME)
SAVE_STATUS_SMOKE_ALIAS := smoke-save-status
ENDGAME_NEW_SURVIVAL_SMOKE_BIN := $(BUILD_DIR)/$(ENDGAME_NEW_SURVIVAL_SMOKE_NAME)
ENDGAME_NEW_SURVIVAL_SMOKE_ALIAS := smoke-endgame-new-survival

SMOKE_SUITES := \
	SAVE_SMOKE \
	MAP_SMOKE \
	TASK_RECIPE_SMOKE \
	TASK_INTERACTION_SMOKE \
	TASK_TEXT_SMOKE \
	SESSION_SMOKE \
	ENDGAME_SMOKE \
	UI_LAYOUT_SMOKE \
	HEALTH_OXYGEN_SMOKE \
	STATUS_SYSTEM_SMOKE \
	CAMP_RECOVERY_SMOKE \
	SAVE_STATUS_SMOKE \
	ENDGAME_NEW_SURVIVAL_SMOKE

$(foreach suite,$(SMOKE_SUITES),$(eval $(call define_binary_target,$(suite))))

APP_BIN := $(BUILD_DIR)/$(APP_NAME)

.PHONY: all
all: $(APP_BIN)

$(APP_BIN): $(APP_OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(APP_OBJECTS) -o $@ $(LDFLAGS)

.PHONY: run xcode clean smoke

run: $(APP_BIN)
	$(APP_BIN)

xcode:
	cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=$(RAYLIB_PREFIX)

smoke: smoke-save smoke-map smoke-task-recipe smoke-task-interaction smoke-task-text smoke-session smoke-endgame smoke-ui-layout smoke-health-oxygen smoke-status-system smoke-camp-recovery smoke-save-status smoke-endgame-new-survival

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPFILES)
