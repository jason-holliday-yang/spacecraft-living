# Tiled 地图源文件迁移计划

目标：先不做体验优化，先让现有地图内容更完整地迁移到 `maps/world.tmj`，方便后续扩大地图、替换美术瓦片和重做整体结构。

## 当前已经在 Tiled 中

- Ground：地面
- Props：障碍/交互物
- Area：区域类型
- Hazard：危险
- Anchors：关键点位
- Resources：资源点
- Monsters：怪物点
- Logs：日志点

这些已经可以在 Tiled 里查看和编辑。

## 当前仍受代码限制

### 1. 地图尺寸仍是代码常量

当前：

- `MAP_WIDTH = 126`
- `MAP_HEIGHT = 104`

位置：`include/config.h`

影响：不能自由把 Tiled 地图改大。要支持大地图，需要让运行时读取 `world.tmj` 的 width/height，或先同步修改常量和数组。

### 2. 地点名仍在代码里判断

例如：

- West Frontier
- Survey Break
- Canopy Hollow
- Echo Basin
- Last Camp
- South Collapse
- Vent Galleries
- Service Shafts
- Purifier Ring
- Root Vault
- Deep Basin
- Monolith Ring
- Signal Tower Plateau

主要位置：

- `src/world/map/map_runtime.c`
- `src/app/session/game_route_runtime.cpp`
- `src/content/task_content.cpp`
- `src/gameplay/task/task_survival_support.c`

影响：大范围移动区域时，地点名和任务路线可能不会跟着 Tiled 自动变化。

### 3. 门、屏障、捷径逻辑仍部分写死

例如：

- Swamp gate
- Ruins gate
- Rope barriers
- Boss arena entry

影响：Tiled 里移动屏障后，代码解锁逻辑可能仍用旧坐标。

### 4. 旧代码地图生成仍保留为回退

位置：

- `src/world/map/map_layout_world.c`
- `src/world/map/map_layout_ship.c`

影响：这是安全回退，不必马上删除。但后续如果 Tiled 成为唯一源文件，可以逐步弱化。

## 推荐迁移顺序

### 阶段 A：保持固定尺寸，完成 Tiled 源文件化

目标：在 126x104 下，让现有地图内容尽量都由 Tiled 控制。

任务：

1. 新增 `Regions` 对象层：保存地点名矩形。
2. 新增 `Unlocks` 对象层：保存门、屏障、捷径。
3. 让 `Map_GetLocationNameAt` 优先读取 `Regions`。
4. 让门/屏障解锁优先读取 `Unlocks` 或 Anchors。
5. 保留旧代码作为回退。

### 阶段 B：替换美术瓦片支持

目标：让 Tiled tileset 更接近正式美术。

任务：

1. 扩展 tileset，不只用色块预览。
2. 保持 tile enum 映射稳定。
3. 增加装饰层，例如 `Decor` 或 `Overlay`。
4. 运行时渲染支持更多非阻挡装饰。

### 阶段 C：支持更大地图

目标：允许 `world.tmj` 扩大尺寸。

任务：

1. 把 `GameMap` 中固定二维数组改成动态分配，或设定更大上限。
2. `Map_IsWithinBounds` 改为读取当前地图尺寸。
3. 存档、寻路、minimap、UI map panel 同步支持动态尺寸。
4. 测试覆盖不同地图尺寸。

### 阶段 D：Tiled 成为唯一地图源

目标：删除或弱化旧代码生成地图。

任务：

1. Tiled 加载失败时显示明确错误。
2. 旧生成器只保留为开发工具或测试 fixture。
3. 文档明确所有地图改动必须从 Tiled 开始。

## 当前建议

下一步不要继续体验优化。

建议先做：

1. `Regions` 对象层。
2. `Map_GetLocationNameAt` 优先读取 Tiled Regions。
3. `Unlocks` / 屏障数据化。

这样后续你改大结构时，地点名和路线逻辑才会更容易跟着地图走。

## 已完成：阶段 A1/A2 基础接入

本轮已完成：

- `GameMap` 增加 `width`、`height`、`tileWidth`、`tileHeight`、`loadedFromTiled`。
- Tiled loader 读取并校验当前地图尺寸与 tile 尺寸。
- `world.tmj` 新增 `Regions` 对象层。
- `GameMap` 增加 `regions`、`regionCount`、`hasRegionLayer`。
- `Map_GetLocationNameAt` 现在优先读取 Tiled Regions，旧代码地点判断保留为回退。
- 测试增加 Tiled 来源、尺寸、Regions 层断言。

验证：

```sh
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke
```

结果：全部通过。

下一步：阶段 A3，迁移 Unlocks / Gates / Barriers。

## 已完成：阶段 A3 Unlocks 基础接入

本轮已完成：

- `world.tmj` 新增 `Unlocks` 对象层。
- `GameMap` 增加 `unlocks`、`unlockCount`、`hasUnlockLayer`。
- Tiled loader 读取 `Unlocks` 对象。
- `Map_CreateRopeBridge` 优先通过 Unlocks 清理对应屏障区域。
- `Map_CanCrossWithRope` 保持兼容，并可识别 Unlocks 管理的屏障。
- 测试增加 Unlocks 层加载断言。

当前 Unlocks 已记录：

- `ROPE_BARRIER_A/B/C`
- `AIRLOCK_DOOR`
- `LOXI_ROOM_DOOR`
- `SWAMP_DEEP_GATE`
- `RUINS_GATE`

注意：本轮只把 rope bridge 的运行时行为优先接入 Unlocks。门和大 gate 的 lock/unlock 函数仍保留旧坐标回退，下一轮继续迁移。

验证：完整 smoke 通过。

## 已完成：阶段 A3 Unlocks 运行时收尾

本轮在 A3 基础接入之上完成了运行时收尾：

- 新增 `Map_GetUnlockById(...)` 和 `Map_GetUnlockAt(...)`，统一查询 Tiled `Unlocks` 对象。
- 新增 `Map_SetUnlockOpen(...)` 和 `Map_IsUnlockOpen(...)`，统一控制门、闸门和绳索屏障状态。
- `AIRLOCK_DOOR`、`LOXI_ROOM_DOOR`、`SWAMP_DEEP_GATE`、`RUINS_GATE` 均改为 Unlocks 优先，旧坐标仅在地图缺少对应对象时回退。
- 绳索屏障继续通过同一套 Unlock API 清理完整对象范围。
- 存档的动态图块捕获和恢复会优先识别 Tiled Unlocks 管理范围，同时保留旧坐标存档兼容。
- 地图 smoke 增加 Unlock ID、范围查询、开关状态和绳索清理测试。

验证：`make smoke` 全部通过。

下一步：阶段 A4，继续清理任务、UI、存档和路线展示中的关键坐标硬编码。

## 已完成：阶段 A4 关键运行时坐标清理

本轮继续完成固定尺寸阶段的运行时数据化：

- 基地氧气台、Loxi 终端、工作台、气闸控制台和能源控制台的交互判定改为 Anchors 优先。
- 任务目标标记中的基地设施、遗迹入口、通讯中继、坠毁线索、能量核心、石碑、信号塔和 BOSS 点改为运行时 Anchors 优先。
- 新增 `RUINS_APPROACH` 锚点，和平路线引导不再依赖唯一硬编码坐标。
- 新增按名称查询地图 Regions 的接口；地图 UI 和 BOSS 攻击范围优先读取 `Northwest Ruins` 区域边界。
- 工作台附近判定和 Loxi 终端确认范围改为地图锚点优先。
- 旧坐标宏仍保留在默认锚点、旧地图生成器、旧存档兼容和内容表回退中。

验证：地图、任务交互、任务文本 smoke 和完整应用构建通过。

## 进行中：阶段 B Decor 图层

已完成第一批运行时基础：

- `world.tmj` 新增非阻挡 `Decor` tile layer。
- 新增独立 `spacecraft_decor.tsj`，固定 `firstgid = 301`。
- 新增碎片、草丛、芦苇、遗迹裂纹和警示灯五类装饰枚举。
- Tiled loader、`GameMap`、查询 API 和地图渲染管线已经接入 Decor。
- Decor 不参与碰撞、寻路和交互，仅在 Ground/Hazard 之后、Props 之前绘制。
- 地图 smoke 已覆盖 Decor 图层和首批示例装饰点。

当前仍需继续：用正式美术替换 Decor 预览 tileset，并根据完整试玩扩大装饰数据覆盖。


# 最终实施状态（2026-07-16）

原计划的阶段 A-D 已完成代码实施：

- **阶段 A 完成**：Ground、Props、Area、Hazard、Anchors、Resources、Monsters、Logs、Regions、Unlocks 均进入运行时；任务、UI、门和关键坐标使用数据层优先。
- **阶段 B 完成**：新增独立 Decor tileset、非阻挡 Decor 图层、加载器、查询 API 和渲染管线。正式美术可以直接替换 tileset 图片，不需要再次修改地图逻辑。
- **阶段 C 完成**：GameMap 五个 tile layer 和 MiniMap 改为运行时尺寸动态分配；加载器、渲染、任务、UI 和边界判断读取实际宽高；新增 4×3 Tiled fixture；存档升级为 V15 并保存地图宽高和 32 位动态图块坐标。
- **阶段 D 完成**：`maps/world.tmj` 成为正式运行的唯一地图源；核心图层不完整时进入显式错误状态并显示错误画面；旧代码生成器只通过 `MapInternal_InitLegacyFixture` 保留为内部开发/测试 fixture。

当前正式地图仍为 `126 × 104`，但这已经是 `world.tmj` 的内容选择，不再是运行时数组限制。tile 尺寸仍固定为 `96 × 96`。

验证要求：

```sh
make clean
make smoke
make -j2
```

另外，`tests/fixtures/dynamic_map.tmj` 用于证明不同地图尺寸、动态 minimap 和图层末端读写。

---

## 多地图阶段 0～3 补充（2026-07-19）

Tiled 唯一来源原则已扩展到多地图：`map_catalog.json` 只负责稳定地图 ID 和文件路径，具体 Ground/Decor/Props/Area/Hazard、Anchor、资源、怪物、日志、Region、Unlock、Portal 和大型实体信息仍全部来自对应 `.tmj`。

新增对象层：

- `Portals`：保存 `portalId`、`targetMapId`、`targetAnchor`、`transitionId`、`interactionRadius`、`promptKey` 和可选 `requiresFlag`。
- `Entities`：保存 `entityId`、`entityType`、`textureKey`、`originAnchor`、可选 `portalId`、`drawLayer` 和 `blocksMovement`。

室外飞船视觉与对象矩形由 Entities 描述，主要碰撞 footprint 由 Props Tile Layer 描述，`blocksMovement=true` 让同一 Entity 对象矩形成为运行时安全碰撞；入口由 Portals 描述，出生/对齐位置由 Anchors 描述。代码不得为飞船占地、入口或出生点建立第二套固定坐标数据。
