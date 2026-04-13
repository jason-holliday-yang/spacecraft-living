# src/ 说明

`src/` 放运行时代码实现，以及只供实现层使用的 internal header。这里不再按“每个文件逐条展开”，而是按模块族说明，方便快速定位。

## 目录分组

- `game_*`
  顶层状态机、会话流、overlay 输入流、运行时协调和渲染编排。
- `map_*`
  地图初始化、世界布局、飞船布局、运行时查询和地图绘制切片。
- `player_*`
  玩家移动、基础数值、状态系统、消耗品和资源库存。
- `task_*`
  任务初始化、推进、生存规则、目标查询、交互、战斗、合成和文本内容。
- `ui_*`
  HUD、菜单、叙事面板、日志、地图、背包、合成和存档槽等界面切片。
- `save_*`
  设置与存档编解码、旧版本兼容、快照装载、存储访问。
- `assets* / audio_system.cpp / localization.cpp / minimap.cpp / puzzle.cpp / resource_path.cpp`
  资源加载、音频、文本、本地化、小型辅助系统和内容支持模块。

## 关键 internal header

- `game_manager_internal.h`
  顶层协调层共享的私有声明。
- `game_session_internal.h`
  会话流和存档流共享 helper。
- `game_overlay_internal.h`
  前台 / 游戏内 overlay 的共享输入 helper。
- `game_play_internal.h`
  进行中游戏循环的共享边界。
- `map_internal.h`
  地图布局、运行时和渲染切片的共享私有接口。
- `task_runtime_internal.h`
  任务运行时、交互和合成模块共享接口。
- `ui_runtime_internal.h`
  UI 切片共享的布局与绘制基础函数。
- `ui_inventory_internal.h`
  背包 / 合成面板共享数据和 helper。
- `ui_story_internal.h`
  叙事面板内部数据与绘制辅助。
- `save_internal.h` 与 `save_codec_internal.h`
  存档 codec 的内部 API 与底层 buffer helper。
- `assets_internal.h`
  资源系统的加载与生命周期内部边界。

## 当前结构约定

- `C` 侧优先保留核心运行时和高频模拟逻辑。
- `C++` 侧优先承载资源、UI、持久化、内容表和高层编排。
- 新功能优先拆成小模块，不再往超大文件继续堆实现。
- 兼容边界文件可以保留旧入口语义，但实现应继续收束到更窄的模块切片。

## 维护规则

- internal header 不应被外部模块当成 public API 使用。
- 当实现拆分发生变化时，优先更新这里和对应头文件顶部说明。
- 某个接口只有在被多个外部模块稳定依赖时，才考虑提升到 `include/`。
