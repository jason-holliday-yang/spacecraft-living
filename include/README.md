# include/ 说明

`include/` 只放“跨模块可见的 public header”，默认保持 C ABI 兼容，供 `C` 和 `C++` 两侧共同引用。

## 分类

- 运行时核心状态
  - `game_manager.h`
  - `map.h`
  - `player.h`
  - `task_system.h`
  - `ui_system.h`
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

## 维护规则

- 能不暴露到 `include/` 的声明，就不要放进来
- `include/` 中的头文件优先保持稳定接口，不承载实现细节
- 私有协调层、模块拆分辅助声明优先放到 `src/*internal.h`
