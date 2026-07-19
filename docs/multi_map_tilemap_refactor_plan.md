# SpaceCraft Living 多地图 TileMap 重构计划

日期：2026-07-19

## 1. 重构目标

把当前单张 `maps/world.tmj` 拆分为互不连续、可以独立加载的 TileMap 场景：

- `ship_interior`：飞船内部，保留现有舱室结构和玩法。
- `planet_surface_01`：飞船外部，重新设计为独立探索地图。
- 玩家通过气闸/飞船交互，在两张地图之间切换。
- 切换期间播放插画、淡入淡出或短暂加载画面。
- 室外的飞船显示为大型贴图，但地形、碰撞、交互入口和出生点仍由 Tiled 数据控制。
- 后续可以继续增加空间站、洞穴、遗迹、其他星球等独立 TileMap，而不修改核心切换逻辑。

核心原则：**TileMap/Tiled 继续作为地图内容的唯一来源，代码只负责加载、运行时状态、切换和通用规则，不重新回到固定坐标硬编码。**

---

## 2. 当前项目基础与限制

### 已经可以直接复用

当前地图数据化工作已经完成了以下基础：

- `maps/world.tmj` 已成为正式地图源。
- Ground、Decor、Props、Area、Hazard 五个 tile layer 已进入运行时。
- Anchors、Resources、Monsters、Logs、Regions、Unlocks 对象层已进入运行时。
- `GameMap` 已支持从 Tiled 读取动态 width/height。
- MiniMap 已支持动态地图尺寸。
- 存档 V15 已保存地图尺寸和 32 位动态图块坐标。
- 地图加载失败已经有显式错误状态。
- 关键设施、任务点、门和屏障多数已改为 Tiled 数据优先。

因此，本轮不需要重新实现 TileMap 加载器，而是在现有加载器上增加“地图身份”和“多文件切换”。

### 当前仍然默认只有一张地图

主要限制如下：

1. `Map_Init` 固定加载 `maps/world.tmj`。
2. `Game` 只有一个 `GameMap map`，没有当前地图 ID、目标地图和地图切换请求。
3. `Map_GetAreaAt`、`Map_GetLocationNameAt`、运行时 Anchor/Region 等接口依赖全局活动地图缓存。
4. `TaskSystem` 中的资源、怪物和日志只有坐标，没有所属地图 ID。
5. 存档只保存一组玩家坐标和一张地图的动态图块状态。
6. 当前屏幕过渡系统只能执行固定菜单/睡眠/结局动作，没有地图切换动作。
7. 当前 `world.tmj` 把飞船内部和整个室外世界放在同一坐标系中。

这些是本次多地图重构的真正工作重点。

---

## 3. 目标结构

建议最终数据结构如下：

```text
maps/
├── map_catalog.json
├── ship_interior.tmj
├── planet_surface_01.tmj
├── legacy_world.tmj              # 迁移期保留，完成后可归档
├── tilesets/
│   ├── spacecraft_tiles.tsj
│   ├── spacecraft_areas.tsj
│   ├── spacecraft_hazards.tsj
│   ├── spacecraft_decor.tsj
│   └── planet_surface_tiles.tsj  # 室外正式瓦片，可分阶段加入
└── transitions/
    └── transition_defs.json      # 可选，首版也可先写在地图对象属性中
```

### 地图目录 `map_catalog.json`

每张地图使用稳定字符串 ID，不使用简单的 `isOutside` 布尔值：

```json
{
  "maps": [
    {
      "id": "ship_interior",
      "file": "maps/ship_interior.tmj",
      "kind": "interior",
      "defaultSpawn": "PLAYER_START",
      "respawnAnchor": "PLAYER_RESPAWN",
      "minimapEnabled": true
    },
    {
      "id": "planet_surface_01",
      "file": "maps/planet_surface_01.tmj",
      "kind": "exterior",
      "defaultSpawn": "SHIP_EXIT_SPAWN",
      "respawnAnchor": "SHIP_EXIT_SPAWN",
      "minimapEnabled": true
    }
  ]
}
```

这样未来新增地图时，核心代码不需要增加新的 `if/else`。

---

## 4. Tiled 图层规范

两张地图继续使用当前核心图层：

| 图层 | 类型 | 职责 |
|---|---|---|
| Ground | Tile Layer | 基础地面与可行走地板 |
| Decor | Tile Layer | 不阻挡的视觉装饰 |
| Props | Tile Layer | 墙体、障碍、设施碰撞和交互图块 |
| Area | Tile Layer | 区域身份、环境和规则分类 |
| Hazard | Tile Layer | 绊倒、沼泽、毒区等危险 |
| Anchors | Object Layer | 出生点、任务点、设施点 |
| Resources | Object Layer | 资源生成点 |
| Monsters | Object Layer | 怪物出生点 |
| Logs | Object Layer | 日志点 |
| Regions | Object Layer | 地点名称范围 |
| Unlocks | Object Layer | 门、屏障和可改变地图结构 |

新增两个通用对象层：

### Portals

负责地图切换入口，不把目标地图写死在代码中。

建议属性：

| 属性 | 示例 | 说明 |
|---|---|---|
| `portalId` | `SHIP_AIRLOCK_EXIT` | 当前入口唯一 ID |
| `targetMapId` | `planet_surface_01` | 目标地图 |
| `targetAnchor` | `SHIP_EXIT_SPAWN` | 目标出生锚点 |
| `transitionId` | `exit_ship` | 使用的过场类型 |
| `interactionRadius` | `1` | 可交互距离 |
| `promptKey` | `EXIT_SHIP` | 本地化交互提示 |
| `requiresFlag` | 可为空 | 可选剧情/装备条件 |

### Entities

负责大型、非单格的地图视觉实体，例如室外飞船。

室外飞船对象建议属性：

| 属性 | 示例 |
|---|---|
| `entityId` | `PLAYER_SHIP` |
| `entityType` | `ship_exterior` |
| `textureKey` | `ship_exterior_default` |
| `originAnchor` | `SHIP_EXTERIOR_ORIGIN` |
| `portalId` | `SHIP_EXTERIOR_ENTRY` |
| `drawLayer` | `above_props` |

飞船虽然使用大型贴图显示，但其占地碰撞仍建议画在 `Props` Tile Layer 中。这样玩家寻路、碰撞、地图编辑和贴图更换保持解耦：

- Entities：画飞船整张贴图。
- Props：定义飞船不可穿越的 tile footprint。
- Portals：定义舱门交互区域。
- Anchors：定义玩家出舱和回舱后的落点。

这仍然是以 TileMap 为核心，而不是把室外地图改成自由摆放图片的场景。

---

## 5. 分阶段实施计划

## 阶段 0：冻结并验证当前单地图基线

### 目标

保护当前未提交的 Tiled 数据化成果，避免多地图重构和现有重构混在一起后难以定位问题。

### 工作

- 记录当前 `world.tmj` 的尺寸、图层和对象数量。
- 确认现有 smoke 全部通过。
- 将当前 `world.tmj` 作为迁移参考基线，不直接在其上破坏式拆分。
- 新增多地图重构专项测试入口。

### 验收

```sh
make clean
make smoke
make -j2
```

应用可以继续以原单地图方式运行。

---

## 阶段 1：把加载器从“加载 world”改成“加载任意地图”

### 目标

先建立通用地图身份和加载接口，不改变玩家当前看到的内容。

### 主要改造

- `GameMap` 新增：
  - `mapId`
  - `sourcePath`
  - `mapKind`
- 把 `MapInternal_LoadTiledWorld` 改为通用 `MapInternal_LoadTiledMap`。
- 移除加载器内部对 `maps/world.tmj` 的固定 fallback 路径假设。
- 新增地图目录读取和查询 API。
- `Map_Init` 暂时仍默认加载旧主地图，保证阶段内表现不变。

### 重点文件

- `include/map.h`
- `src/world/map/map_internal.h`
- `src/world/map/map_layout.c`
- `src/world/map/map_tiled_loader.c`
- `src/infrastructure/assets/resource_path.cpp`
- `CMakeLists.txt`
- `Makefile`

### 验收

- 同一个加载接口能分别加载两个 fixture。
- 不同地图可以拥有不同尺寸。
- 图层缺失时错误信息包含 map ID 和文件路径。
- 原地图 smoke 不退化。

---

## 阶段 2：清理“全局唯一活动地图”假设

### 目标

让所有区域、地点、Anchor、Region 查询明确针对某个 `GameMap`，避免地图切换后仍读到上一张地图的数据。

### 主要改造

优先新增或替换以下 map-aware API：

```c
MapArea Map_GetAreaAt(const GameMap *map, int gridX, int gridY);
const char *Map_GetLocationNameAt(const GameMap *map, int gridX, int gridY);
const char *Map_GetRoomNameAt(const GameMap *map, int gridX, int gridY);
bool Map_GetAnchorPosition(const GameMap *map, MapAnchor anchor, int *x, int *y);
const MapRegion *Map_GetRegionByName(const GameMap *map, const char *name);
```

迁移完成前可以保留旧 wrapper，但新地图切换代码不得依赖全局缓存。

### 需要检查的系统

- 地图渲染
- 玩家移动和危险判定
- 任务目标标记
- 怪物 AI
- 生存系统
- 地点名称 UI
- 音频场景选择
- MiniMap
- 存档恢复安全坐标检查

### 验收

- 连续加载地图 A、地图 B 后，对 A 的显式查询仍返回 A 的数据。
- 不会因最近一次加载地图不同而改变测试结果。
- 当前地图全部玩法保持一致。

---

## 阶段 3：拆分地图数据，但暂不重做室外布局

### 目标

先完成“数据分家”，再做关卡重设计，避免同时修改架构和大量地图内容。

### 数据迁移策略

1. 创建 `ship_interior.tmj`。
2. 创建临时 `planet_surface_legacy.tmj`。
3. 从当前 `world.tmj` 分别复制对应 tile 和对象数据。
4. `world.tmj` 在迁移期改名或复制为 `legacy_world.tmj`，作为回归参考。

### 飞船内部处理建议

第一轮拆分时**先保留现有内部坐标**，不要同时裁剪和平移整个舱室。原因：当前仍有部分旧坐标回退、复合物件边界和任务内容依赖原坐标。

当地图切换、任务、存档稳定后，再单独执行“内部地图紧凑化”：

- 裁掉大量无用空白。
- 平移舱室到新地图中心。
- 同步 Anchors、Regions、Resources、Logs、Unlocks。
- 删除对应硬编码 fallback。

### 室外临时地图

临时室外地图只用于保持旧任务链可玩，不作为最终设计：

- 去除飞船内部 tile。
- 原飞船位置替换为 `PLAYER_SHIP` 大型实体。
- 在飞船附近增加安全落地区。
- 添加 `SHIP_EXTERIOR_ENTRY` Portal。
- 原外部区域和任务点暂时保留。

### 验收

- 两张 `.tmj` 均可单独打开和校验。
- 室内不加载室外资源、怪物、日志和区域。
- 室外不加载室内设施、房间名和室内资源。
- 飞船贴图占地不可穿越，舱门附近至少存在一个安全可走格。

---

## 阶段 4：建立地图场景管理器

### 目标

让 `Game` 可以安全切换活动 TileMap。

### 建议新增

- `include/map_scene.h`
- `src/world/map/map_scene_manager.c`
- `MapSceneManager` 或 `WorldMapManager`

建议状态：

```text
currentMapId
previousMapId
pendingTargetMapId
pendingTargetAnchor
pendingTransitionId
transitionPhase
mapLoadError
```

### 切换流程

```text
玩家与 Portal 交互
→ 校验条件
→ 锁定移动和其他交互
→ 开始过场/淡出
→ 保存当前地图运行时状态
→ 加载目标 TileMap 到临时 GameMap
→ 校验目标 Anchor 和安全落点
→ 成功后替换 game->map
→ 重建 MiniMap
→ 激活目标地图实体
→ 将玩家放到目标 Anchor
→ 重置相机和移动动画
→ 淡入并恢复输入
```

必须使用“先加载临时地图，成功后再交换”的方式。目标地图加载失败时，保留旧地图并显示错误，不把玩家留在半初始化状态。

### Game 层修改

- `ScreenTransitionAction` 新增地图切换动作，或者新增独立 `MapTransitionState`。
- 地图切换不要复用“新游戏/读档”逻辑重置玩家和任务。
- `Game_ResetGameplayWorld` 只用于新游戏，不用于地图切换。
- 地图切换后执行：
  - `MiniMap_Destroy/Init`
  - `Player_UpdateWorldPosition`
  - `Game_ResetCameraToPlayer`
  - 清空移动输入缓冲
  - 刷新地点名、音频场景和任务标记

### 验收

- 室内点击气闸可进入室外。
- 玩家出现在室外飞船旁，而不是沿用室内坐标。
- 点击室外飞船可回到室内气闸附近。
- 连续往返 20 次不崩溃、不泄漏、不重复初始化任务。
- 玩家背包、生命、氧气、任务阶段保持不变。

---

## 阶段 5：地图局部实体和任务归属

### 目标

解决资源、怪物、日志和任务标记在多地图中的归属问题。

### 数据模型

给以下运行时对象增加稳定的地图归属：

- `ResourceNode.mapId`
- `Monster.mapId`
- `ShipLog.mapId`
- 任务目标：`targetMapId + targetAnchor/targetObjectId`

### 运行规则

- 只绘制当前地图所属实体。
- 只让当前地图怪物更新和碰撞。
- 当前任务目标在其他地图时：
  - 室内显示“前往气闸/离开飞船”。
  - 室外显示实际目标位置。
- 地图切换不重新生成已经采集的资源或已经击败的怪物。
- 室外营地只属于对应室外地图。

### 推荐后续拆分

当前 `TaskSystem` 同时承担叙事进度和世界实体容器。首轮可增加 `mapId` 过滤，降低改动风险；稳定后再考虑拆为：

```text
QuestProgress       全局任务/剧情状态
WorldEntityState    资源、怪物、日志运行时状态
MapSceneManager     当前地图加载和切换
```

不要在第一轮多地图重构中同时完成这个大拆分。

### 验收

- 室内看不到室外资源、怪物和目标标记。
- 室外看不到室内日志和设施交互。
- 切图后已采集资源、怪物血量和日志收集状态正确。
- 跨地图任务能正确引导玩家先到出口，再指向真实目标。

---

## 阶段 6：过场插画与加载表现

### 目标

完成用户可感知的出舱/回舱体验。

### 首版表现

出舱：

```text
交互确认
→ 0.2~0.35 秒淡出
→ 显示出舱插画和短文案
→ 黑屏阶段加载目标地图
→ 0.2~0.35 秒淡入室外
```

回舱使用另一张插画或同一张插画的不同裁切/文案。

### 降级策略

- 插画资源缺失时使用纯黑淡入淡出和文字，不阻塞地图功能。
- 当前地图较小时不需要伪造长加载时间。
- 过场播放期间停止移动、攻击、交互和暂停菜单嵌套操作。

### 可复用现有系统

- `screenTransitionActive`
- `screenTransitionElapsed`
- `Game_GetScreenTransitionAlpha`
- 现有 Story Scene/Cutscene 绘制框架

地图切换应拥有独立 transition action，不能冒充睡眠或读档。

### 验收

- 过场中不能移动或重复触发 Portal。
- 地图只在完全遮黑或插画覆盖阶段交换。
- 加载失败能从过场返回原场景并显示明确错误。

---

## 阶段 7：存档升级为多地图版本

### 目标

保存当前地图以及每张地图的局部变化。

### 建议升级

存档从 V15 升级到 V16，新增：

```text
currentMapId
currentPlayerGridX/currentPlayerGridY
lastPositionByMap[]（可选）
mapStateCount
mapStates[]
```

每个 `MapStateSnapshot` 至少保存：

- `mapId`
- 地图版本或内容校验标识
- 动态清除/恢复的 Props tile
- Unlock 状态
- 营地位置
- 资源节点状态
- 怪物状态
- 日志状态
- 可选 MiniMap 探索状态

### 旧存档迁移

V15 旧存档来自当前合并地图：

- 根据玩家坐标和 Area/Region 判断初始地图。
- 位于飞船 Base 区域的玩家迁移到 `ship_interior`。
- 其他位置迁移到 `planet_surface_legacy` 或新室外地图的兼容 Anchor。
- 如果旧坐标在新地图不可用，使用 `Game_FindNearestSafeLoadedPlayerTile`。
- 无法可靠映射时使用目标地图的 `PLAYER_RESPAWN`，但不能丢失背包和任务进度。

### 验收

- 在室内保存并读取，仍在室内正确位置。
- 在室外保存并读取，仍在室外正确位置。
- 室外打开的门、采集的资源、击败的怪物不会因进出飞船重置。
- V15 smoke 存档可以迁移到 V16。
- 地图尺寸改变后仍有安全坐标修复。

---

## 阶段 8：重新设计正式室外地图

### 目标

在多地图架构稳定后，开始真正的外部关卡重做。

### 不建议直接覆盖临时地图

新建 `planet_surface_01_v2.tmj` 或在独立分支文件中设计，完成可玩性验收后再替换目录指向。这样旧地图始终可以用于对照和回归。

### 室外地图第一轮灰盒结构

建议围绕飞船形成以下层次：

1. **降落安全区**
   - 飞船是最强视觉地标。
   - 玩家出生点安全、无遮挡。
   - 返回舱门容易识别。
   - 附近只有低风险基础资源。

2. **近距离探索环**
   - 从飞船附近形成 2~3 条方向明确的出口。
   - 每条路线都能通过地形、Decor 和地标辨认。
   - 至少形成一条可回到飞船的环路，避免只有狭长走廊。

3. **中风险功能区**
   - 沼泽、设施废墟、调查营地等开始分化。
   - 每个区域有不同 Ground、Props、Hazard 和资源结构。
   - 关键任务点附近保留低/中风险站位。

4. **高风险深区**
   - 使用 Unlocks、装备条件或任务阶段控制进入。
   - 高价值资源、怪物和剧情目标形成风险回报。
   - 设计清晰回撤路线，不单纯堆叠毒区。

5. **远景与地图边界**
   - 用不可通行 TileMap、悬崖、密林、深水或遗迹墙形成自然边界。
   - 不再让地图边缘表现为简单矩形截断。

### TileMap 设计要求

- 地图尺寸由 `.tmj` 决定，不新增宽高常量。
- tile 尺寸首轮继续保持 `96 × 96`，避免同时重做移动、碰撞和全部美术比例。
- 先完成灰盒 Ground/Props/Area/Hazard，再填 Decor 和正式素材。
- 每个主区域至少有一个 Region 和一个 Anchor。
- 任务内容引用 Anchor/Region/Object ID，不引用绝对坐标。
- 室外飞船的贴图可以更换，但 Portal、碰撞 footprint 和出生点不随贴图耦合。

### 室外设计验收

- 从飞船出发 10 秒内能理解至少一个探索方向。
- 玩家从附近区域可以凭视觉地标找到飞船。
- 主路线不是唯一单通道，至少有一条回环或捷径。
- 危险强度从安全区向深区逐步增加。
- 关键交互点不存在全相邻格不可站立的问题。
- 地图边界在画面中表现为自然环境，而不是裸露矩形边框。

---

## 6. 推荐实施顺序

严格按以下顺序推进：

```text
现有重构基线验证
→ 通用地图加载
→ map-aware 查询 API
→ 地图文件拆分
→ Portal / Entities 数据层
→ 室内外切换纵向切片
→ 地图实体归属过滤
→ 多地图存档 V16
→ 正式室外灰盒重做
→ 美术和关卡精修
```

不要同时进行以下三件事：

- 改多地图底层架构；
- 大幅移动所有任务点；
- 替换全部正式瓦片美术。

先让最小闭环稳定：

```text
室内气闸
→ 过场
→ 小型室外测试地图
→ 飞船贴图
→ 返回室内
→ 保存/读取仍正确
```

闭环通过后，再逐区重做正式室外地图。

---

## 7. 测试计划

建议新增：

### `map_catalog_smoke`

- 地图 ID 唯一。
- 文件存在。
- 每张地图核心图层完整。
- 默认出生 Anchor 存在且可走。

### `map_scene_smoke`

- 室内加载。
- 室外加载。
- A → B → A 切换。
- 玩家状态不重置。
- MiniMap 尺寸正确更新。

### `map_portal_smoke`

- Portal 目标地图存在。
- 目标 Anchor 存在。
- 出生点不是障碍或怪物占位。
- 室内和室外 Portal 成对。

### `map_state_smoke`

- Unlock 状态跨切图保留。
- 资源采集状态跨切图保留。
- 怪物状态跨切图保留。
- 营地只恢复在所属地图。

### `save_multi_map_smoke`

- 室内 V16 round-trip。
- 室外 V16 round-trip。
- V15 → V16 迁移。
- 无效位置安全修复。

每个阶段继续保留现有：

```sh
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke-save_system
make smoke
```

---

## 8. 主要风险与控制方式

### 风险 1：任务系统仍把全世界当作一张地图

控制：先给实体和目标增加 `mapId`，用过滤方式迁移；不要立即重写整个任务系统。

### 风险 2：旧坐标回退在内部地图裁剪后失效

控制：第一轮拆分保留内部坐标；稳定后再单独裁剪和平移。

### 风险 3：地图切换导致资源和怪物重新初始化

控制：切图只替换活动 `GameMap`，不调用 `Game_ResetGameplayWorld` 或 `Tasks_Init`。

### 风险 4：加载失败后旧地图已被销毁

控制：目标地图先加载到临时对象，通过完整校验后再交换。

### 风险 5：飞船贴图破坏 TileMap 碰撞

控制：飞船视觉使用 Entities，大型占地碰撞仍由 Props tile footprint 管理。

### 风险 6：存档版本升级范围过大

控制：先完成无存档的往返纵向切片，再设计 V16；保留 V15 解码器和迁移测试。

---

## 9. 第一轮实际开发范围

第一轮建议只交付以下内容，不立即重做完整室外地图：

1. 通用地图目录和加载接口。
2. `ship_interior.tmj`。
3. 一个小型 `planet_surface_test.tmj`。
4. Tiled `Portals` 和 `Entities` 层。
5. 室内气闸 → 过场 → 室外飞船旁。
6. 室外飞船 → 过场 → 室内气闸。
7. 玩家状态、任务状态不重置。
8. MiniMap 正确切换。
9. 最基础的多地图存档字段和往返 smoke。

完成这一轮后，才开始正式室外地图的区域规划和 TileMap 灰盒设计。

---

## 10. 完成定义

地图重构完成需要同时满足：

- 飞船内部和外部是两个独立 `.tmj` 文件。
- 两张地图都由同一个通用 TileMap 加载器加载。
- 出舱和回舱通过 Tiled Portal 数据驱动。
- 室外飞船为大型贴图实体，碰撞与入口仍由 TileMap/Tiled 数据定义。
- 切图不会重置玩家、任务、资源、怪物和地图动态状态。
- 存档明确记录当前地图和每张地图的局部状态。
- 新增第三张地图时，无需修改现有室内/室外切换核心代码。
- 正式室外地图不再表现为一个缺少自然边界的简单矩形场地。

---

## 11. 2026-07-19 第一轮（阶段 0～3）实施记录

本轮已完成：

- 冻结 `world.tmj`，新增与其逐字节一致的 `legacy_world.tmj`。
- 核实现有基线：`126 × 104` 动态地图、动态 MiniMap、Tiled 对象层、存档 V15。
- 新增 `maps/map_catalog.json`；`Map_Init` 经目录加载临时默认 `legacy_world`。
- `GameMap` 增加 `mapId`、`sourcePath`、`mapKind`。
- 新增 `MapCatalog_Load`、`MapCatalog_Find`、`Map_LoadTiledMap`、`Map_LoadById`。
- 移除 Tiled 加载器内部只针对 `maps/world.tmj` 的固定 fallback 列表。
- 新增显式地图查询 API：Area、Location、Room、Region、Anchor 均可绑定 `const GameMap *`；旧 runtime wrapper 仅保留兼容。
- 新增 `Portals` / `Entities` 运行时结构、加载和字段校验。
- 拆分 `ship_interior.tmj` 与 `planet_surface_legacy.tmj`，室内保留原坐标，室外仍是迁移兼容版本。
- 新增 `multi_map_smoke`，覆盖目录、地图 ID、任意尺寸、显式查询独立性、Portal、Entity、双地图加载和错误诊断。

本轮刻意未完成：

- 不执行 Portal 地图切换。
- 不新增地图场景管理器和过场状态。
- 不给任务实体正式增加 `mapId` 归属。
- 不升级存档到 V16；继续使用并验证 V15。
- 不正式重做室外布局或大型飞船美术渲染。

下一轮入口应以 `Map_LoadById(tempMap, catalog, targetMapId)` 为加载边界，先完整校验目标地图和 `targetAnchor`，成功后再交换活动 `GameMap`；不得在加载失败时销毁当前地图。

---

## 12. 2026-07-19 第二轮（阶段 4～6）实施记录

### 实际完成

- 新增通用 `MapSceneManager`，记录 `currentMapId`、`previousMapId`、`pendingMapId`、`targetAnchor`、`transitionId`、来源 Portal、切换阶段、临时地图、安全出生点和加载错误。
- 新增独立 `SCREEN_TRANSITION_MAP_CHANGE`，不复用睡眠、读档或结局动作；过场期间现有输入总闸继续阻止移动、攻击和交互，场景管理器同时拒绝重复 Portal 请求。
- Portal 交互完全读取 Tiled 的 `targetMapId`、`targetAnchor`、`transitionId`、`interactionRadius`、`portalId` 和 `requiresFlag`。当前室内出口不再设置隐藏阶段门槛，玩家靠近气闸即可看到 `F` 交互提示并测试完整往返；目标地图和出生坐标没有写死在交互分支中。
- 完成“临时加载后交换”：目标地图先进入 `MapSceneManager.stagedMap`，校验场景核心图层、目标 Anchor、安全无 Hazard 出生点和实体注册容量；成功后才交换活动 `GameMap` 并销毁旧地图。失败时重新激活原地图 runtime 查询缓存，原地图不变并保留明确错误。
- 新游戏实际活动地图改为 `ship_interior`。初始化时仅执行一次 `Tasks_Init`，随后预注册 `planet_surface_legacy` 的地图局部实体；正常切图不调用 `Game_ResetGameplayWorld` 或 `Tasks_Init`。
- 切换成功后统一重建 MiniMap、同步玩家 Grid/World/Render 坐标和朝向、清理移动动画与输入缓冲、重置相机、刷新地点名、音频 Scene/Music Stage 和任务目标文本。
- `ResourceNode`、`Monster`、`ShipLog` 增加稳定 `mapId`；日志同时保存稳定 `sourceIndex`。`TaskSystem` 记录活动地图和已注册地图列表，同一地图重复注册为幂等操作。
- 资源交互/重生、怪物更新/碰撞/攻击目标、日志交互和世界绘制均按活动 `mapId` 过滤。兼容地图 `legacy_world` 仍视为可显示全部旧实体，以维持 V15 行为。
- 已采集资源、已击败怪物和已收集日志保留在同一个 `TaskSystem` 中，切图只改变活动地图过滤，不重新生成或重建这些数组。
- 新增跨地图任务标记路由：实际目标不属于当前地图时，先指向当前地图中通往目标地图的 Portal；进入目标地图后再指向真实 Anchor、资源或日志。
- 室外 `PLAYER_SHIP` 由 Entities 对象决定位置、尺寸、类型、关联 Portal 和绘制层；Props 继续提供不可穿越占地。`ship_exterior_default` 已映射到带透明背景的 `entity_player_ship_exterior_alpha.png`，纹理缺失时仍保留程序化大型飞船作为安全降级。
- 出舱/回舱过场复用现有淡入淡出框架，采用黑屏、双语短文字、遮黑阶段加载、淡入的可靠降级方案；本轮没有新增正式过场插画。
- V15 读取路径保留：由于 V15 没有 `currentMapId` 和逐地图状态，旧存档加载后进入 `legacy_world` 兼容地图，不删除旧解码器。

### 实际切换流程

```text
玩家在当前地图靠近/点击 Portal
→ 读取 Tiled Portal 数据并建立 pending request
→ SCREEN_TRANSITION_MAP_CHANGE 淡出并锁定输入
→ stagedMap 临时加载目标 mapId
→ 校验核心图层、targetAnchor、安全 Tile、实体容量
→ 注册目标地图实体（已注册则不重复）
→ 原子交换活动 GameMap，激活新地图查询缓存
→ 应用任务进度对应的 Unlock 状态
→ 设置玩家坐标/朝向并清理动画与输入
→ 销毁并重建 MiniMap，重置相机/地点/音频/目标
→ 淡入，场景管理器回到 idle
```

失败路径：任何加载或校验失败都会进入 `MAP_TRANSITION_ERROR`，保留旧 `GameMap`、玩家位置和全局状态，并通过过场/HUD 显示错误。

### 第二轮专项测试

- `map_scene_smoke`：双向 Portal、临时加载、原子交换、重复触发拒绝、20 次完整往返、出生 Anchor、目标文件缺失时保留原图。
- `map_entity_scope_smoke`：双地图实体注册幂等、稳定总数/顺序、资源/怪物 `mapId` 过滤、采集状态跨地图保持、双向分段任务引导。
- `game_session_smoke`：真实 Game 层连续往返 20 次，验证背包、生命、氧气、任务阶段和实体注册次数保持不变。
- 原有 `multi_map_smoke`、`map_layout_smoke`、任务、会话、结局、存档与总 smoke 继续作为回归基线。

### 第三轮必须处理：V16 多地图存档

V16 至少需要新增：

1. `currentMapId`、玩家当前地图坐标与朝向。
2. 每张地图独立的动态图块/Unlock 状态，键应为 `mapId + tile coordinate` 或稳定 Unlock ID。
3. 每张地图的 MiniMap 探索数据，或明确的可重建/压缩策略。
4. 资源状态使用 `mapId + stable seed/object ID`，保存 active、剩余重生次数、重生计时。
5. 怪物状态使用 `mapId + stable spawn/object ID`，保存 active、位置、生命、阶段和必要 AI 状态。
6. 日志状态使用 `mapId + sourceIndex/object ID`，避免仅依赖数组下标。
7. 营地状态增加所属 `mapId`；当前 V15 仍只有单个 camp 坐标。
8. V15 → V16 迁移规则：V15 默认落在 `legacy_world`，或在可可靠判断时迁移到 `ship_interior` / `planet_surface_legacy`；必须保留 V15 解码测试。
9. 存档时若地图切换进行中，应拒绝保存或只保存最后一个已提交活动地图，不能保存 staged 半状态。

### 第三轮及后续室外地图设计问题

- `planet_surface_legacy` 仍保持旧 `126 × 104` 坐标和旧路线，只是兼容切片，不是正式室外地图。
- 正式室外地图应重新设计飞船着陆区轮廓、自然边界、舱门安全站位、探索节奏、区域连通和视觉地标。
- `PLAYER_SHIP` 当前是程序化占位视觉；后续需提供正式大图资源，并明确纹理原点、遮挡层、阴影、舱门开口和玩家前后景关系。
- 正式地图应给 Resource/Monster/Log/Entity 增加稳定对象 ID，减少 V16 对数组顺序和坐标的依赖。
- 两张拆分地图目前尺寸相同是迁移期选择；正式室外地图可使用不同尺寸，继续依赖现有动态 MiniMap 重建路径。

---

## 13. 2026-07-19 第三轮（阶段 7～8）最终实施状态

### 阶段 7：多地图存档 V16 —— 已完成

当前原生存档 magic 已升级为 `SCLSAV16`，V5～V15 解码器全部保留。V16 在完整 V15 全局进度载荷之后追加多地图数据，因此玩家属性、背包、任务、剧情、日志档案和结局字段继续沿用已经验证的编码顺序，同时新增：

- `sourceVersion`（解码后的来源版本，仅运行时使用）；
- `currentMapId`；
- 当前玩家 Grid 坐标与朝向（继续使用原字段）；
- `mapStateCount`；
- `mapStates[]`。

每个 `SavedMapStateSnapshot` 保存：

- `mapId`；
- `contentVersion`；
- 保存时地图宽高；
- Unlock 数量、稳定 `unlockId` 和开关状态；
- 被动态清除的 Props 坐标；
- 营地开关和坐标；
- 该地图资源节点数量、激活状态和剩余重生次数；
- 该地图怪物数量、激活状态、Grid 坐标、生命值和阶段触发状态；
- 该地图日志数量和收集状态。

地图切换前，`MapSceneManager` 会捕获当前地图的 Unlock、动态 Props 和营地状态；目标地图先加载到临时 `GameMap`，随后应用该地图已经保存的局部状态，再执行原子交换。资源、怪物和日志继续保存在全局 `TaskSystem` 中，但 V16 编码时按 `mapId` 分组，读档时按地图内稳定种子顺序恢复。因此进出飞船不会重新生成资源、复活怪物、关闭门或移除营地。

本轮**未保存 MiniMap 探索位图**。原因是当前 `MiniMap.explored` 是只属于活动地图的临时二维分配，切图时会重建，尚无逐地图探索缓存、内容版本重采样规则或地图尺寸变化迁移语义。把该裸位图直接塞入 V16 会让存档与当前尺寸强耦合。V16 先保证影响玩法的地图状态稳定；后续应以按 `mapId + contentVersion` 编码的压缩 bitset 独立升级，而不是改变本轮已稳定的地图状态格式。

### V15 → V16 迁移规则

- 解码器识别 `SCLSAV15` 后将 `sourceVersion` 标记为 15，不删除或改写旧字段。
- 迁移时加载 `legacy_world`，用旧玩家坐标查询旧 Area：
  - `MAP_AREA_BASE` → `ship_interior`；
  - 其他 Area 或无法识别的区域 → `planet_surface_01`；
- 先尝试保留旧 Grid 坐标；如果该坐标在目标地图中越界、碰撞、危险或被实体占据，则从地图目录的 `respawnAnchor` 开始寻找最近安全格；正式室外使用 `SHIP_EXIT_SPAWN`。
- 背包、生命/氧气/状态、死亡次数、装备、任务阶段、支线、修复等级、日志、剧情档案、怪物/资源旧数组和结局状态继续按 V15 数据恢复。
- `legacy_world` 仍可读取 V16 兼容测试存档，但正常新游戏、Portal 和正式室外存档均使用拆分地图。

### 每张地图的局部状态

| 地图 | V16 局部状态 |
|---|---|
| `ship_interior` | `LOXI_ROOM_DOOR` Unlock/Props 状态；室内资源节点；室内日志；室内地图内容版本与尺寸；若未来允许室内营地也可由同一结构保存。气闸现在是固定 Portal 阀门，不再保存开关状态。 |
| `planet_surface_01` | 绳索屏障、深沼门、遗迹门；动态清除 Props；室外营地；全部室外资源节点；怪物激活/位置/生命/阶段；室外日志；地图内容版本与尺寸。 |
| `planet_surface_legacy` | 继续保留目录和加载兼容；若载入并保存，也使用同一通用地图状态结构。 |
| `legacy_world` | 仅用于旧单图回归、V15 区域判定和兼容测试；V16 仍能保存其旧式局部状态，避免测试或开发存档被强制丢弃。 |

### 阶段 8：正式室外 TileMap 首版灰盒 —— 已完成

新增 `maps/planet_surface_01.tmj`，未覆盖 `planet_surface_legacy.tmj`。正式室外地图保持 `126 × 104`、`96 × 96` tile，并完整包含 Ground、Decor、Props、Area、Hazard、Anchors、Resources、Monsters、Logs、Regions、Unlocks、Portals、Entities 十三个规范图层。地图顶层属性包含 `contentVersion=1` 和 `designStatus=playable_graybox_v1`。

正式目录仍保留兼容地图，但 `ship_interior` 的 `SHIP_AIRLOCK_EXIT` 已正式指向 `planet_surface_01`；新游戏预注册的室外实体和跨地图任务默认目标也已切换为 `planet_surface_01`。

### 正式室外区域和路线

- **飞船降落安全区（Landing Safety Zone）**：飞船 Entity 缩为 `6 × 6` tile，并设置 `blocksMovement=true`；Props 在飞船覆盖范围内继续使用不可见 `Entity Blocker`，两者共同保证完整 6×6 碰撞。飞船周围没有树木或岩石；原中央大面积岩石与船舱 Base Floor 已清除，着陆安全区统一改为无 Hazard、无 Decor 的普通森林草地；Portal 和 `SHIP_EXIT_SPAWN` 保持可站立。
- **近距离探索环（Canopy Loop）**：从舱门向东、向北、向南/西南形成三条清楚出口；林地环路绕过飞船与着陆区，并有一条东南捷径重新接回主环，不是单一狭长走廊。
- **西部与南部中风险区**：Echo Basin、Canopy Loop、South Survey Facility 以森林/设施 Ground、树木和岩石 Props、Trip 风险及独立 Regions 区分；提供资源、日志和回撤空间。
- **东部沼泽风险梯度**：Outer Mire 使用沼泽 Ground/Hazard，Deep Mire 使用深沼和 Poison Hazard；`SWAMP_DEEP_GATE` 与绳索屏障控制深区进入，高价值资源、能量核心和调查点位于更深处。
- **北部遗迹链**：Ruins Approach、Monolith Ring、Signal Tower Plateau 使用遗迹 Ground、裂纹 Decor、岩石/废墟 Props 和 `RUINS_GATE`；主线调查点附近均保留低/中风险站位格。
- **西北高风险猎场**：Northwest Hunt Grounds 提供 Boss 入口和出生区，远离飞船安全出生点，并保留向东南撤回林地环路的路线。
- **自然边界**：可玩轮廓在矩形 TMJ 边界内采用正弦起伏的内缩边界，外圈使用 Void、密林和岩壁 Props 叠加，玩家无法走到裸露矩形边缘。

### 仍为灰盒或占位的视觉

- Ground、Area、Hazard、Decor 继续复用现有 spacecraft tileset 色块和程序化绘制，不是最终星球材质。
- 飞船 Entity 已加载独立大型飞船 PNG；碰撞和入口继续由 Props / Portals 控制，后续仍可只替换 `textureKey` 对应资源而不改地图逻辑。
- 遗迹、深沼门、调查设施、悬崖/密林边界主要由现有 Rock、Tree、Barrier、Console tile 组合表达。
- 区域构图、路线、风险层、任务数据和碰撞已达到可玩首版；最终植被层次、地表过渡、独立设施贴图、环境特效和光照仍属于后续美术轮次。

### 第三轮新增验收

- `save_multi_map_smoke`：V16 二进制 round-trip、室外读档、逐地图资源/怪物/日志/Unlock/营地恢复、V15 Base/室外迁移和无效坐标修复。
- `map_state_smoke`：打开遗迹门并放置营地后，进舱再出舱，地图局部状态保持。
- `map_portal_smoke`：正式地图核心图层、内容版本、Portal 目标、目标 Anchor、安全出生点、怪物安全距离、资源/日志站位、自然边界和主任务 Anchor 可达性。
- `map_scene_smoke`：连续 20 次出舱/回舱、原子交换、重复请求拒绝和加载失败保留原地图。
- `map_entity_scope_smoke`：正式室外实体归属与跨地图任务路由。
- `multi_map_smoke`：目录正式指向、飞船 Entity/Props/Portal/Anchor 职责和双地图显式查询。

阶段 7～8 的代码、数据和 smoke 已完成；后续工作应作为视觉精修、MiniMap 探索 bitset 独立版本、更多地图和正式关卡平衡迭代处理，不再阻塞本次多地图 TileMap 重构闭环。

### 2026-07-19 运行体验修正

- 移除 `SHIP_AIRLOCK_EXIT` 的隐藏 `stage_3` 限制；新游戏可以直接走到气闸旁，通过明确的世界高亮和屏幕底部 `F 离开飞船` 提示测试切图。
- 室外 `SHIP_EXTERIOR_ENTRY` 同样显示 `F 进入飞船`，点击大型飞船或按 F 均使用同一 Portal 数据。
- `ship_exterior_default` 已接入 AssetBundle，并加载透明背景飞船纹理；原程序化飞船保留为纹理缺失时的降级视觉。
- 飞船 Entity 从原先覆盖旧飞船内部坐标的大尺寸范围缩为 `6 × 6`，设置 `blocksMovement=true`；正式室外生成器同步清除中央旧岩石占地，并在飞船覆盖范围内写入 36 格不可见 `Entity Blocker`，着陆区周围不再显示树木或岩石。
- 室内删除 Airlock Link、Airlock Console、`AIRLOCK_DOOR` Unlock、开关效果和旧通道残留地板；原通道外资源保留稳定 sourceIndex 并迁入生命维持舱。中央走廊末端外侧重新放置单格气阀贴图，Portal 与气阀共用同一格，返回 Anchor 位于走廊末端。
