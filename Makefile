CMAKE ?= cmake
BUILD_DIR ?= build/cmake
BUILD_TYPE ?= Debug
RAYLIB_ROOT ?= /opt/homebrew
PARALLEL ?=

CMAKE_CONFIGURE_ARGS := \
	-S . \
	-B $(BUILD_DIR) \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DRAYLIB_ROOT=$(RAYLIB_ROOT)

.PHONY: all configure build run smoke bundle xcode clean

all: build

configure:
	$(CMAKE) $(CMAKE_CONFIGURE_ARGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --config $(BUILD_TYPE) $(if $(PARALLEL),--parallel $(PARALLEL),--parallel)

run: configure
	$(CMAKE) --build $(BUILD_DIR) --config $(BUILD_TYPE) --target run

smoke: configure
	$(CMAKE) --build $(BUILD_DIR) --config $(BUILD_TYPE) --target smoke $(if $(PARALLEL),--parallel $(PARALLEL),--parallel)

smoke-%: configure
	$(CMAKE) --build $(BUILD_DIR) --config $(BUILD_TYPE) --target $@ $(if $(PARALLEL),--parallel $(PARALLEL),--parallel)

bundle: configure
	$(CMAKE) --build $(BUILD_DIR) --config $(BUILD_TYPE) --target bundle

xcode:
	$(CMAKE) -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=$(RAYLIB_ROOT)

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
