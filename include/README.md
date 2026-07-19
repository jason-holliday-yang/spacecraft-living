# include/ 公共头文件说明

`include/` 只放跨模块可见、需要稳定 C/C++ 调用边界的 public header。第六阶段只移动实现文件，公共 include 名称保持不变，因此现有调用方无需改写 `#include "map.h"`、`#include "player.h"` 等路径。

## 分类

- 运行时核心状态
  - `game_manager.h`
  - `map.h`
  - `map_scene_manager.h`
  - `player.h`
  - `task_system.h`
  - `ui_system.h`
- 查询与表现边界
  - `map_render.h`
  - `player_presentation.h`
  - `task_presentation.h`
  - `ui_theme.h`
- 资源与平台
  - `assets.h`
  - `audio_system.h`
  - `save_system.h`
  - `persistence_platform.h`
  - `resource_path.h`
- 内容与辅助
  - `recipe_catalog.h`
  - `puzzle.h`
  - `minimap.h`
  - `localization.h`
- 共享基础
  - `config.h`
  - `c_compat.h`

## 实现位置

```text
include/map*.h        -> src/world/map + src/presentation/render
include/player*.h     -> src/gameplay/player + src/presentation/render
include/task*.h       -> src/gameplay/task + src/content + src/presentation/render
include/ui*.h         -> src/presentation/ui
include/save_system.h -> src/infrastructure/save
include/assets.h      -> src/infrastructure/assets
include/audio_system.h-> src/infrastructure/audio
include/game_manager.h-> src/app + src/app/session
```

## 维护规则

- 能不暴露到 `include/` 的声明就不要放进来。
- public header 保持 C ABI 兼容，不承载实现细节或物理目录路径。
- 私有协调层和拆分辅助声明应放到对应模块目录的 `*internal.h`。
- 测试若需要白盒接口，应通过 CMake 内部 include 路径引用 internal header，不新增 public API。
