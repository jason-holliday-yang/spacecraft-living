# Xcode Setup

## Generate the Xcode project

```sh
cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=/opt/homebrew
open build/xcode/SpaceCraftLiving.xcodeproj
```

## Run inside Xcode

1. Select the `SpaceCraftLiving` scheme.
2. Build with `Product -> Build`.
3. Run with `Product -> Run`.

## Resource folder

If you later add PNG art, keep the files inside `resources/`.

When importing the folder into Xcode, use:

- `Create folder references`

This preserves the runtime paths expected by the code.

## Gameplay module map

- `src/player.c`: movement, stamina, oxygen inventory helpers
- `src/map.c`: 100 x 100 map generation, view culling, tile rendering, doors
- `src/task_system.c`: NPC dialogue, repair order, unlock flow, ending condition
- `src/ui_system.c`: HUD, intro screen, ending screens
- `src/game_manager.c`: state machine, main update loop, camera, survival ticking
