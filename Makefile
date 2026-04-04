APP_NAME := SpaceCraftLiving
BUILD_DIR := build
RAYLIB_PREFIX ?= /opt/homebrew

SOURCES := \
	src/main.c \
	src/assets.c \
	src/audio_system.c \
	src/save_system.c \
	src/localization.c \
	src/map.c \
	src/player.c \
	src/task_system.c \
	src/ui_system.c \
	src/game_manager.c \
	src/puzzle.c

CFLAGS := -std=c11 -Wall -Wextra -pedantic -Iinclude -I$(RAYLIB_PREFIX)/include
LDFLAGS := -L$(RAYLIB_PREFIX)/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo

$(BUILD_DIR)/$(APP_NAME): $(SOURCES)
	mkdir -p $(BUILD_DIR)
	clang $(CFLAGS) $(SOURCES) -o $(BUILD_DIR)/$(APP_NAME) $(LDFLAGS)

.PHONY: run clean xcode

run: $(BUILD_DIR)/$(APP_NAME)
	./$(BUILD_DIR)/$(APP_NAME)

xcode:
	cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=$(RAYLIB_PREFIX)

clean:
	rm -rf $(BUILD_DIR)
