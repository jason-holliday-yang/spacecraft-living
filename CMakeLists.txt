cmake_minimum_required(VERSION 3.18)
project(SpaceCraftLiving C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

add_executable(SpaceCraftLiving
    src/main.c
    src/assets.c
    src/audio_system.c
    src/save_system.c
    src/localization.c
    src/map.c
    src/player.c
    src/task_system.c
    src/ui_system.c
    src/game_manager.c
)

target_include_directories(SpaceCraftLiving PRIVATE
    include
    ${RAYLIB_INCLUDE_DIR}
)

target_compile_options(SpaceCraftLiving PRIVATE -Wall -Wextra -pedantic)

if (APPLE)
    target_link_libraries(SpaceCraftLiving PRIVATE
        ${RAYLIB_LIBRARY}
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(SpaceCraftLiving PRIVATE ${RAYLIB_LIBRARY})
endif()
