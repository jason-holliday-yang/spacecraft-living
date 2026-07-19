# SpaceCraft Living 地图数据

这个目录保存可被 Tiled 打开的地图数据。TileMap/Tiled 仍然是地图内容的唯一来源；代码只负责目录查询、加载、运行时状态和通用规则，不重新维护第二套固定坐标地图。

## 文件与当前运行策略

- `map_catalog.json`：稳定地图 ID 到 `.tmj` 文件的目录。低层 `Map_Init` 仍以 `legacy_world` 作为旧测试兼容默认；正常新游戏会由 `Game_ResetGameplayWorld` 切换到 `ship_interior`。
- `world.tmj`：阶段 0 冻结的原单地图基线，不做破坏式覆盖。
- `legacy_world.tmj`：与当前 `world.tmj` 逐字节一致的迁移参考，也是本轮临时 gameplay 默认地图。
- `ship_interior.tmj`：独立飞船内部地图，保留原 `126 × 104` 坐标系，不裁剪、不平移。
- `planet_surface_legacy.tmj`：独立临时室外地图，保留旧室外内容用于迁移兼容，不代表最终室外设计。
- `tilesets/spacecraft_tiles.tsj`：游戏逻辑图块说明，对应 `TileType`。
- `tilesets/spacecraft_areas.tsj`：区域图层说明，对应 `MapArea`。
- `tilesets/spacecraft_hazards.tsj`：危险图层说明，对应 `HazardType`。
- `tilesets/spacecraft_decor.tsj`：装饰图层说明，对应 `MapDecorType`。
- `tilesets/*.png`：给 Tiled 预览用的色块图，不是正式游戏美术资源。

`Map_Init` 现在先读取 `maps/map_catalog.json`，再按目录中的 `defaultMapId` 通用加载地图；不再由加载器固定猜测 `maps/world.tmj`。正常新游戏随后加载 `ship_interior`，玩家在气闸附近会看到 `F 离开飞船` 提示，并通过 `MapSceneManager` 切换到 `planet_surface_01`；室外飞船入口提供对应的返回交互。

## 地图目录格式

目录中的每一项包含：

- `id`：稳定字符串地图 ID；禁止用 `bool isOutside` 代替。
- `file`：资源相对路径。
- `kind`：`interior`、`exterior`、`legacy` 或 `test`。
- `defaultSpawn`：默认出生 Anchor 名称。
- `respawnAnchor`：复活 Anchor 名称。
- `minimapEnabled`：是否允许 MiniMap。

当前登记：

| 地图 ID | 文件 | kind | 用途 |
|---|---|---|---|
| `legacy_world` | `maps/legacy_world.tmj` | `legacy` | 临时完整玩法默认地图 |
| `ship_interior` | `maps/ship_interior.tmj` | `interior` | 独立飞船内部 |
| `planet_surface_legacy` | `maps/planet_surface_legacy.tmj` | `exterior` | 临时旧室外迁移地图 |

地图目录加载会校验 ID 唯一、必要字段、kind、默认地图 ID 和文件存在性。地图加载失败信息必须同时保留地图 ID 与文件路径。

## 通用图层规范

### Tile Layers

- `Ground`：基础地面和可行走地板，对应 `groundTiles`。
- `Decor`：非阻挡视觉装饰，对应 `decorTiles`。
- `Props`：墙体、障碍、设施和大型实体占地碰撞，对应 `propTiles`。
- `Area`：区域身份、环境和规则分类，对应 `areaTiles`。
- `Hazard`：绊倒、沼泽、毒区等危险，对应 `hazardTiles`。

### Object Layers

- `Anchors`：出生点、任务点、设施点和大型实体原点。
- `Resources`：资源生成点。
- `Monsters`：怪物出生点；允许地图保留空层。
- `Logs`：日志点。
- `Regions`：地点名称范围。
- `Unlocks`：门、屏障和可改变地图结构。
- `Portals`：数据驱动的地图入口。第二轮已用于数据驱动的运行时地图切换。
- `Entities`：大型、非单格视觉实体。第二轮已渲染室外飞船大型占位视觉；正式美术仍待替换。

空对象层仍然代表该地图遵循规范，不能用“对象数量为 0”误判为图层缺失。

## Portals 数据格式

每个对象 `type` 必须为 `portal`，建议使用矩形对象，并包含：

| 属性 | 必需 | 说明 |
|---|---|---|
| `portalId` | 是 | 当前地图内唯一入口 ID |
| `targetMapId` | 是 | `map_catalog.json` 中的目标地图 ID |
| `targetAnchor` | 是 | 目标地图 Anchor 名称 |
| `transitionId` | 是 | 下一轮过场/切换流程使用的 ID |
| `interactionRadius` | 是 | 交互距离，必须非负 |
| `promptKey` | 是 | 本地化提示键 |
| `requiresFlag` | 否 | 可选剧情或装备条件 |

当前 `SHIP_AIRLOCK_EXIT` 的 `requiresFlag` 为空，确保多地图往返从新游戏开始即可直接验证，不再存在不可见的 `stage_3` 门槛。

加载器校验必需字段、重复 ID、对象范围和尺寸；专项 smoke 还会加载目标地图并验证目标 Anchor 存在且可走。

## Entities 数据格式

每个对象 `type` 必须为 `entity`，并包含：

| 属性 | 必需 | 说明 |
|---|---|---|
| `entityId` | 是 | 当前地图内唯一实体 ID |
| `entityType` | 是 | 例如 `ship_exterior` |
| `textureKey` | 是 | 后续视觉资源键 |
| `originAnchor` | 是 | 对齐大型贴图的 Anchor |
| `portalId` | 否 | 与实体关联的入口 |
| `drawLayer` | 是 | 例如 `above_props` |
| `blocksMovement` | 否 | 大型实体自身是否按对象矩形阻挡移动 |

室外飞船遵循四层职责分离：

- `Entities`：大型飞船视觉信息；
- `Props`：飞船不可穿越的 tile footprint；
- `Portals`：进入飞船的交互入口；
- `Anchors`：出舱和回舱后的出生位置及实体原点。

`PLAYER_SHIP` 同时设置 `blocksMovement=true`。因此即使旧存档或运行时地图状态意外清掉某个不可见 blocker，Entity 的 6×6 对象矩形仍会阻挡玩家和视线，不会再次出现穿过飞船的情况。

`PLAYER_SHIP.textureKey=ship_exterior_default` 当前映射到：

```text
resources/images/world/entities/entity_player_ship_exterior_alpha.png
```

该资源使用透明背景，由 AssetBundle 统一加载和释放；若纹理缺失，运行时仍绘制程序化飞船轮廓作为降级，不会再次出现完全空白的 Entity 区域。

## 本轮两张拆分地图

### `ship_interior.tmj`

保留图层：`Ground`、`Decor`、`Props`、`Area`、`Hazard`、`Anchors`、`Resources`、`Monsters`（空层）、`Logs`、`Regions`、`Unlocks`、`Portals`、`Entities`（空层）。

对象数量：Anchors 7、Resources 7、Monsters 0、Logs 3、Regions 8、Unlocks 1、Portals 1、Entities 0。旧 Airlock Link 通道、Airlock Console、五格舱门 Unlock 和旧通道残留地板已经移除；原本落在通道外的资源保留稳定编号并移入生命维持舱。中央走廊末端外侧保留一个单格 `TILE_AIRLOCK_DOOR` 气阀贴图，Portal 与气阀使用同一格，玩家站在走廊末端按 F 即通过 `SHIP_AIRLOCK_EXIT` 切换地图。室内只保留 `LOXI_ROOM_DOOR` Unlock。

### `planet_surface_legacy.tmj`

保留图层：`Ground`、`Decor`、`Props`、`Area`、`Hazard`、`Anchors`、`Resources`、`Monsters`、`Logs`、`Regions`、`Unlocks`、`Portals`、`Entities`。

对象数量：Anchors 21、Resources 62、Monsters 8、Logs 11、Regions 24、Unlocks 5、Portals 1、Entities 1。移除室内设施、房间 Region、室内资源/日志和室内门；保留旧室外任务链数据。原飞船区域改为 `PLAYER_SHIP` Entity，并用 Props 定义碰撞，占地东侧保留安全入口与 `SHIP_EXIT_SPAWN`。

### `planet_surface_01.tmj` 当前飞船区

- `PLAYER_SHIP` Entity 固定为 `6 × 6` tile。
- 原先 `40..82 × 43..61` 的大面积岩石和船舱地板填充已删除。
- 中央着陆区 `40..90 × 42..64` 以及延伸到 `x=97` 的着陆安全区统一使用普通 `TILE_FOREST_GROUND` 草地；该范围无 Hazard、无 Decor、无随机树木或岩石。
- Props 在飞船覆盖的 6×6 范围内使用不可见 `Entity Blocker`，共 36 格；飞船周围不再放置岩石。
- `SHIP_EXTERIOR_ENTRY` 位于飞船东侧，`SHIP_EXIT_SPAWN` 位于入口外一格。
- 重新运行 `tools/generate_planet_surface_01.py` 会保留这套尺寸和清理规则。

## Tiled 编辑规则

1. 地图宽高可以不同，由 `.tmj` 的 `width` / `height` 决定；tile 尺寸当前必须保持 `96 × 96`。
2. 不要删除或改名规范图层。拆分地图允许某些对象层为空，但图层本身应保留。
3. 不要改变 tileset 顺序和 `firstgid`：基础 tiles 从 `1` 开始，Area 从 `101` 开始，Hazard 从 `201` 开始，Decor 从 `301` 开始。
4. 移动 Anchor 时，对象 `x` / `y` 和属性 `gridX` / `gridY` 要同步。
5. Portal 的目标必须使用地图 ID + Anchor，不写死目标坐标。
6. 大型实体视觉不承担碰撞；碰撞必须留在 Props。
7. `ship_interior.tmj` 第一轮禁止整体裁剪和平移，直到旧坐标回退、任务、复合物件和存档迁移完成。
8. `planet_surface_legacy.tmj` 只用于兼容，不在其上直接完成正式室外地图设计。

## 查询与测试

新代码优先使用显式地图查询：

- `Map_GetAreaAtMap(map, x, y)`
- `Map_GetLocationNameAtMap(map, x, y)`
- `Map_GetRoomNameAtMap(map, x, y)`
- `Map_GetRegionAt(map, x, y)` / `Map_GetRegionByName(map, name)`
- `Map_GetAnchorPosition(map, anchor, ...)`

旧的无 `map` wrapper 仅保留迁移兼容。加载地图 A、B 后，针对 A 的显式查询不得受“最近加载地图”影响。

地图改动后至少运行：

```sh
make smoke-map_layout
make smoke-multi_map
make smoke-task_interaction
make smoke-endgame_flow
make smoke-save_system
make smoke
make -j2
```

## 第二轮运行时边界

阶段 4～6 已完成：`MapSceneManager` 使用临时 `GameMap` 加载和校验 Portal 目标，成功后才交换活动地图；切换会重建 MiniMap，并按 `mapId` 过滤资源、怪物和日志。出舱/回舱使用独立地图过场状态。

第三轮仍需完成 V16 多地图存档：当前 V15 没有当前地图 ID、逐地图动态图块、探索数据和营地归属，旧存档继续加载到 `legacy_world` 兼容地图。`planet_surface_legacy.tmj` 仍不是正式室外设计。

## 第三轮正式室外地图

`planet_surface_01.tmj` 是当前正式室外地图，`ship_interior.tmj` 的出舱 Portal 已指向该地图。`planet_surface_legacy.tmj` 继续保留为坐标、任务和视觉回归参考，不应覆盖。

正式室外地图顶层属性：

- `contentVersion = 1`：V16 地图局部状态兼容标识；
- `designStatus = playable_graybox_v1`：完整可玩首版灰盒。

`tools/generate_planet_surface_01.py` 可重建当前灰盒数据布局。修改生成规则后必须重新运行 `map_portal_smoke`、`map_state_smoke`、`map_scene_smoke`、`save_multi_map_smoke` 和完整 `make smoke`。

MiniMap 探索位图暂不进入 V16；原因和后续压缩 bitset 方案记录在 `docs/multi_map_tilemap_refactor_plan.md` 第 13 节。

### 飞船 Entity 碰撞 Tile

`TILE_ENTITY_BLOCKER` / tileset id `26` 是碰撞专用 Props Tile：

- Tiled 预览图块为透明；
- 运行时 `MapInternal_DrawPropCell` 不绘制它；
- `Map_IsWalkable` 将其视为不可通行；
- 当前 `PLAYER_SHIP` 的 6×6 覆盖范围全部使用该 Tile，因此玩家不能穿过飞船，同时地图上不会出现伪装成飞船碰撞的岩石。

### 多格大型地标贴图

通信中继、飞船残骸、能源核心、信号塔、石碑以及船内大型设施会直接读取 Props 图层中连续的矩形占地。渲染器只在该矩形左上角绘制一次完整贴图，并缩放到整个 footprint，不再把对象中心 Anchor 错当成左上角后在每个 tile 上重复绘制。

地图生成器不会再为了给资源或交互留站位而清除地标内部格子：旧资源如果落在地标 footprint 内，会保留稳定 `sourceIndex` 并移动到地标外最近的安全格；交互站位也只清理矩形外侧。
