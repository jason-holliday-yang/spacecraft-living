# src/ 最终目录说明

`src/` 按稳定模块边界组织。公共 ABI 位于仓库根目录的 `include/`；私有头文件与实现文件放在同一模块目录，不再平铺到 `src/` 根目录。

## 目录树

```text
src/
├── app/
│   ├── session/
│   └── game_* / main.c
├── gameplay/
│   ├── player/
│   └── task/
├── world/
│   └── map/
├── presentation/
│   ├── ui/
│   └── render/
├── infrastructure/
│   ├── assets/
│   ├── audio/
│   └── save/
└── content/
```

## 模块职责

- `app/`
  应用入口、顶层 `Game` 协调、活动帧、输入和 Overlay 分发。
- `app/session/`
  新游戏、存档恢复、地图切换、消息、音频场景、转场和状态清理。
- `gameplay/player/`
  玩家移动、基础数值、持续状态、消耗品和资源库存。
- `gameplay/task/`
  任务初始化、推进、生存规则、目标查询、交互、战斗、制作和结局规则。
- `world/map/`
  地图动态存储、Tiled 加载、目录与场景管理、运行时查询、Region、Anchor 和 Unlock。旧代码布局只用于显式 legacy fixture。
- `presentation/render/`
  游戏世界、地图、MiniMap、玩家与任务表现；不拥有玩法状态。
- `presentation/ui/`
  UI 主题、组件、布局、HUD、菜单和所有 Overlay 面板。
- `infrastructure/assets/`
  AssetBundle 生命周期、资源 IO 和可执行文件相对路径解析。
- `infrastructure/audio/`
  音效与音乐加载、播放和场景切换底层。
- `infrastructure/save/`
  存档/设置 codec、存储访问、旧版本兼容和平台持久化。
- `content/`
  本地化、配方、谜题、任务文本和静态内容表。

## CMake 边界

每个目录对应一个或多个 object library：

```text
scl_assets_obj       scl_audio_obj       scl_content_obj
scl_map_obj          scl_map_render_obj  scl_player_obj
scl_task_obj         scl_save_obj        scl_session_obj
scl_ui_obj           scl_render_obj      scl_app_obj
          \              |              /
                 spacecraft_internal
                    /             \
          SpaceCraftLiving      smoke tests
```

`CMakeLists.txt` 是唯一的源文件与目标清单。根目录 Makefile 只调用 CMake，不允许重新维护第二套源文件列表。

## Internal header 规则

- internal header 只供当前实现和白盒 smoke 测试使用。
- 跨目录引用 internal header 时，由 CMake 的内部 include 路径提供，不使用 `../src/...` 相对路径。
- `world/map/map_internal.h` 只保留小型共享几何 helper；布局、加载、Region 和 Anchor 分别使用窄 internal header。
- UI、任务、存档、会话和资源私有声明保留在各自目录。
- 只有被多个外部模块稳定依赖且需要 C ABI 的接口才提升到 `include/`。

## 依赖方向

- `app/session` 可以协调 gameplay、world、infrastructure 和 presentation。
- presentation 可以读取 gameplay/world/content，不反向拥有玩法状态。
- gameplay 不依赖 UI 或 AssetBundle。
- world/map 只依赖公共地图类型、Tiled 数据和资源路径解析。
- save 通过公共运行时结构制作快照，不依赖 UI 绘制实现。
