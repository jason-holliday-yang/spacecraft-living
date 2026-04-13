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

If you add PNG or audio assets, keep them under `resources/`.

When importing the folder into Xcode, use:

- `Create folder references`

This preserves the runtime paths expected by the code.

The runtime also searches for `resources/` relative to the executable location, so assets can still be found when Xcode launches the binary from a build directory instead of the project root.

## Related Docs

- [`ARCHITECTURE.md`](ARCHITECTURE.md) for the current module map
- [`CPP_MIGRATION_PLAN.md`](CPP_MIGRATION_PLAN.md) for the current C / C++ ownership split
