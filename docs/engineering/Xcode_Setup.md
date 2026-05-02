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

If Xcode shows `Process completed` immediately and no game window appears, the usual cause is that
the active scheme is one of the generated `SpaceCraft*Smoke`, `RUN_TESTS`, `ALL_BUILD`, or
`ZERO_CHECK` targets instead of `SpaceCraftLiving`. Those helper targets are expected to exit
quickly. Switch the scheme back to `SpaceCraftLiving` and run again.

## Important

- Open `build/xcode/SpaceCraftLiving.xcodeproj`.
- Do not open the repository root as an Xcode project.
- Do not add `build/`, `build-*`, or `DerivedData` folders into the Xcode navigator or any target.

The repository contains many generated `.d` dependency files under `build/`.
If those files are imported into Xcode, Xcode can classify them as DTrace inputs and create
`CompileDTraceScript` build steps. Because those generated files are not real source inputs and
often share output locations, Xcode's new build system can then fail with
`Multiple commands produce ...` errors in `DerivedData`.

## Troubleshooting `CompileDTraceScript` / `Multiple commands produce`

If Xcode shows `CompileDTraceScript` or `Multiple commands produce ... DerivedData ...` while
building `SpaceCraftLiving`, clean the project state before trying again:

1. Close Xcode.
2. Delete any manually created or stale Xcode project/workspace that points at the repo root.
3. Remove `build/` file references from Xcode if they were added previously, especially any
   `*.d` files shown in `Build Phases -> Compile Sources`.
4. Regenerate the CMake Xcode project:

```sh
rm -rf build/xcode
cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=/opt/homebrew
open build/xcode/SpaceCraftLiving.xcodeproj
```

5. In Xcode, run `Product -> Clean Build Folder`.
6. If the error still persists, delete the matching `SpaceCraftLiving-*` folder under:

```text
~/Library/Developer/Xcode/DerivedData
```

## Resource folder

If you add PNG or audio assets, keep them under `resources/`.

When importing the folder into Xcode, use:

- `Create folder references`

This preserves the runtime paths expected by the code.

The runtime also searches for `resources/` relative to the executable location, so assets can still be found when Xcode launches the binary from a build directory instead of the project root.

## Related Docs

- [`ARCHITECTURE.md`](ARCHITECTURE.md) for the current module map
- [`CPP_MIGRATION_PLAN.md`](CPP_MIGRATION_PLAN.md) for the current C / C++ ownership split
