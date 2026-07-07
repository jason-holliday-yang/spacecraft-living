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

TASK_CONTENT_CXX_SOURCES := \
	src/recipe_catalog.cpp \
	src/task_content.cpp \
	src/task_content_data.cpp \
	src/puzzle.cpp \
	src/task_actions.cpp \
	src/task_interactions.cpp \
	src/task_ship_interactions.cpp \
	src/task_world_interactions.cpp \
	src/task_crafting.cpp

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
	src/game_audio_scene.cpp \
	src/game_messages.cpp \
	src/game_route_runtime.cpp \
	src/game_runtime.cpp \
	src/game_save_flow.cpp \
	src/game_save_runtime.cpp \
	src/game_session.cpp \
	src/game_state_sanitize.cpp \
	src/game_transition.cpp

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

TASK_SMOKE_C := $(MAP_C_SOURCES) $(PLAYER_C_SOURCES) $(TASK_RUNTIME_C_SOURCES)
TASK_SMOKE_CXX := $(TASK_CONTENT_CXX_SOURCES) src/localization.cpp

SAVE_SMOKE_SOURCES := tests/save_system_smoke.c $(MAP_C_SOURCES) $(SAVE_CXX_SOURCES) src/localization.cpp
MAP_SMOKE_SOURCES := tests/map_layout_smoke.c $(MAP_C_SOURCES)
TASK_RECIPE_SMOKE_SOURCES := tests/task_recipe_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)
TASK_INTERACTION_SMOKE_SOURCES := tests/task_interaction_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)
TASK_TEXT_SMOKE_SOURCES := tests/task_text_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)
SESSION_SMOKE_SOURCES := tests/game_session_smoke.c $(MAP_C_SOURCES) $(PLAYER_C_SOURCES) src/game_play_story.c $(TASK_RUNTIME_C_SOURCES) src/audio_system.cpp src/localization.cpp src/minimap.cpp $(TASK_CONTENT_CXX_SOURCES) src/task_presentation.cpp $(SAVE_CXX_SOURCES) $(SESSION_CXX_SOURCES) src/resource_path.cpp
ENDGAME_SMOKE_SOURCES := tests/endgame_flow_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)
UI_LAYOUT_SMOKE_SOURCES := tests/ui_layout_smoke.c $(UI_RUNTIME_C_SOURCES) src/ui_layout.cpp
HEALTH_OXYGEN_SMOKE_SOURCES := tests/health_oxygen_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)
STATUS_SYSTEM_SMOKE_SOURCES := tests/status_system_smoke.c $(MAP_C_SOURCES) $(PLAYER_C_SOURCES) src/recipe_catalog.cpp src/localization.cpp
CAMP_RECOVERY_SMOKE_SOURCES := tests/camp_recovery_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)
SAVE_STATUS_SMOKE_SOURCES := tests/save_status_smoke.c $(MAP_C_SOURCES) $(SAVE_CXX_SOURCES) src/localization.cpp
ENDGAME_NEW_SURVIVAL_SMOKE_SOURCES := tests/endgame_new_survival_smoke.c $(TASK_SMOKE_C) $(TASK_SMOKE_CXX)

BUILD ?= debug

ifeq ($(BUILD),release)
  OPT_FLAGS := -O2 -DNDEBUG
else
  OPT_FLAGS := -g -O0
endif

CPPFLAGS := -Iinclude -I$(RAYLIB_PREFIX)/include
DEPFLAGS := -MMD -MP
CFLAGS := -std=c11 $(OPT_FLAGS) -Wall -Wextra -pedantic $(DEPFLAGS)
CXXFLAGS := -std=c++17 $(OPT_FLAGS) -Wall -Wextra -pedantic $(DEPFLAGS)
LDFLAGS := -L$(RAYLIB_PREFIX)/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo

define smoke_target
$(BUILD_DIR)/$(1)_smoke: $$(call objects_for,smoke_$(1),$(2))
	mkdir -p $(BUILD_DIR)
	$(CXX) $$^ -o $$@ $(LDFLAGS)

.PHONY: smoke-$(1)
smoke-$(1): $(BUILD_DIR)/$(1)_smoke
	$(BUILD_DIR)/$(1)_smoke
endef

objects_for = \
	$(patsubst src/%.c,$(BUILD_DIR)/$1/%.o,$(filter src/%.c,$2)) \
	$(patsubst src/%.cpp,$(BUILD_DIR)/$1/%.o,$(filter src/%.cpp,$2)) \
	$(patsubst tests/%.c,$(BUILD_DIR)/$1/tests_%.o,$(filter tests/%.c,$2))

APP_OBJECTS := $(call objects_for,app,$(APP_C_SOURCES) $(APP_CXX_SOURCES))
DEPFILES := $(APP_OBJECTS:.o=.d)

$(BUILD_DIR)/app/%.o: src/%.c Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/app/%.o: src/%.cpp Makefile
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

define smoke_compile_rules
$(BUILD_DIR)/$(1)/%.o: src/%.c Makefile
	mkdir -p $$(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $$< -o $$@

$(BUILD_DIR)/$(1)/%.o: src/%.cpp Makefile
	mkdir -p $$(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $$< -o $$@

$(BUILD_DIR)/$(1)/tests_%.o: tests/%.c Makefile
	mkdir -p $$(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $$< -o $$@
endef

$(eval $(call smoke_compile_rules,smoke_save_system))
$(eval $(call smoke_compile_rules,smoke_map_layout))
$(eval $(call smoke_compile_rules,smoke_task_recipe))
$(eval $(call smoke_compile_rules,smoke_task_interaction))
$(eval $(call smoke_compile_rules,smoke_task_text))
$(eval $(call smoke_compile_rules,smoke_game_session))
$(eval $(call smoke_compile_rules,smoke_endgame_flow))
$(eval $(call smoke_compile_rules,smoke_ui_layout))
$(eval $(call smoke_compile_rules,smoke_health_oxygen))
$(eval $(call smoke_compile_rules,smoke_status_system))
$(eval $(call smoke_compile_rules,smoke_camp_recovery))
$(eval $(call smoke_compile_rules,smoke_save_status))
$(eval $(call smoke_compile_rules,smoke_endgame_new_survival))

$(eval $(call smoke_target,save_system,$(SAVE_SMOKE_SOURCES)))
$(eval $(call smoke_target,map_layout,$(MAP_SMOKE_SOURCES)))
$(eval $(call smoke_target,task_recipe,$(TASK_RECIPE_SMOKE_SOURCES)))
$(eval $(call smoke_target,task_interaction,$(TASK_INTERACTION_SMOKE_SOURCES)))
$(eval $(call smoke_target,task_text,$(TASK_TEXT_SMOKE_SOURCES)))
$(eval $(call smoke_target,game_session,$(SESSION_SMOKE_SOURCES)))
$(eval $(call smoke_target,endgame_flow,$(ENDGAME_SMOKE_SOURCES)))
$(eval $(call smoke_target,ui_layout,$(UI_LAYOUT_SMOKE_SOURCES)))
$(eval $(call smoke_target,health_oxygen,$(HEALTH_OXYGEN_SMOKE_SOURCES)))
$(eval $(call smoke_target,status_system,$(STATUS_SYSTEM_SMOKE_SOURCES)))
$(eval $(call smoke_target,camp_recovery,$(CAMP_RECOVERY_SMOKE_SOURCES)))
$(eval $(call smoke_target,save_status,$(SAVE_STATUS_SMOKE_SOURCES)))
$(eval $(call smoke_target,endgame_new_survival,$(ENDGAME_NEW_SURVIVAL_SMOKE_SOURCES)))

APP_BIN := $(BUILD_DIR)/$(APP_NAME)

.PHONY: all run xcode clean smoke bundle

all: $(APP_BIN)

$(APP_BIN): $(APP_OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

run: $(APP_BIN)
	$(APP_BIN)

xcode:
	cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=$(RAYLIB_PREFIX)

smoke: smoke-save_system smoke-map_layout smoke-task_recipe smoke-task_interaction smoke-task_text smoke-game_session smoke-endgame_flow smoke-ui_layout smoke-health_oxygen smoke-status_system smoke-camp_recovery smoke-save_status smoke-endgame_new_survival

clean:
	rm -rf $(BUILD_DIR)

BUNDLE_DIR := $(BUILD_DIR)/$(APP_NAME).app
BUNDLE_CONTENTS := $(BUNDLE_DIR)/Contents
BUNDLE_MACOS := $(BUNDLE_CONTENTS)/MacOS
BUNDLE_RESOURCES := $(BUNDLE_CONTENTS)/Resources

bundle: $(APP_BIN)
	mkdir -p $(BUNDLE_MACOS)
	cp $(APP_BIN) $(BUNDLE_MACOS)/$(APP_NAME)
	cp Info.plist $(BUNDLE_CONTENTS)/Info.plist
	mkdir -p $(BUNDLE_RESOURCES)
	ln -sfn "$(shell pwd)/resources" $(BUNDLE_RESOURCES)/resources
	@echo "App bundle created at $(BUNDLE_DIR)"

-include $(DEPFILES)
