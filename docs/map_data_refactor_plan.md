# SpaceCraft Living 地图数据化重构计划

> 目的：把当前硬编码地图逐步迁移为 Tiled/数据驱动地图，方便后续由 AI 或人工在地图文件里调整地形、区域、危险、锚点、资源、怪物和事件点。

## 当前状态快照

截至当前工作进度，地图重构已经完成以下阶段：

### 已完成 1：Tiled 地图文件基础接入

已新增：

- `maps/world.tmj`
- `maps/README.md`
- `maps/tilesets/spacecraft_tiles.tsj`
- `maps/tilesets/spacecraft_areas.tsj`
- `maps/tilesets/spacecraft_hazards.tsj`
- `maps/tilesets/*.png`
- `src/world/map/map_tiled_loader.c`

当前 `world.tmj` 已包含：

- `Ground` 地面层
- `Props` 物体层
- `Area` 区域层
- `Hazard` 危险层
- `Anchors` 关键点位层
- `Resources` 资源点层
- `Monsters` 怪物出生点层
- `Logs` 日志点层

游戏启动时：

1. 优先读取 `maps/world.tmj`
2. 如果读取失败，回退旧的代码生成地图

相关文件：

- `src/world/map/map_layout.c`
- `src/world/map/map_tiled_loader.c`
- `src/world/map/map_internal.h`
- `Makefile`
- `CMakeLists.txt`

---

### 已完成 2：Area / Hazard 图层运行时接入

`GameMap` 已新增：

- `areaTiles[MAP_HEIGHT][MAP_WIDTH]`
- `hazardTiles[MAP_HEIGHT][MAP_WIDTH]`
- `hasAreaLayer`
- `hasHazardLayer`

运行时逻辑：

- `Map_GetAreaAt(...)` 优先使用当前激活地图的 `Area` 图层
- `Map_GetHazardAt(...)` 优先使用 `Hazard` 图层
- 图层缺失时回退旧坐标/地形判断逻辑

相关文件：

- `include/map.h`
- `src/world/map/map_runtime.c`
- `src/world/map/map_tiled_loader.c`
- `tests/map_layout_smoke.c`

---

### 已完成 3：Anchors 锚点层接入

`world.tmj` 已新增 `Anchors` 对象层，当前有 27 个锚点：

- `PLAYER_START`
- `PLAYER_RESPAWN`
- `AIRLOCK_DOOR`
- `AIRLOCK_EXIT`
- `LOXI_TERMINAL`
- `OXYGEN_CONSOLE`
- `WORKBENCH`
- `AIRLOCK_CONSOLE`
- `ENERGY_CONSOLE`
- `COMM_RELAY`
- `CRASH_CLUE`
- `ENERGY_CORE`
- `SIGNAL_TOWER`
- `BOSS_PLAYER_ENTRY`
- `BOSS_SPAWN`
- `MONOLITH_A`
- `MONOLITH_B`
- `MONOLITH_C`
- `SWAMP_OUTER_GATE`
- `SWAMP_DEEP_GATE`
- `RUINS_GATE`
- `ROPE_BARRIER_A`
- `ROPE_BARRIER_B`
- `ROPE_BARRIER_C`
- `WEST_ECHO_BASIN`
- `SOUTH_FACILITY`
- `FINAL_GATE`

`GameMap` 已新增：

- `hasAnchorLayer`
- `anchorSet[MAP_ANCHOR_COUNT]`
- `anchorX[MAP_ANCHOR_COUNT]`
- `anchorY[MAP_ANCHOR_COUNT]`

新增接口：

- `Map_GetAnchorName(...)`
- `Map_GetAnchorPosition(...)`

当前策略：

- 地图文件有 `Anchors` 层时读取地图锚点
- 缺失时使用旧坐标默认锚点
- 暂时还没有大规模替换业务代码里的旧坐标宏，后续逐步替换

---

### 已完成 4：Resources 资源点层接入

`world.tmj` 已新增 `Resources` 对象层，当前有 69 个有效资源点。

每个资源点包含属性：

- `resourceType`
- `resourceName`
- `baseYield`
- `respawnsRemaining`
- `special`
- `gridX`
- `gridY`
- `sourceIndex`

`GameMap` 已新增：

- `hasResourceLayer`
- `resourceSeeds[MAX_RESOURCE_NODES]`
- `resourceSeedCount`

任务初始化逻辑已改成：

1. 如果地图有 `Resources` 层，优先使用地图资源点
2. 如果地图缺失资源层，回退旧的 `TasksContent_GetNodeSeed(...)` 资源表

相关文件：

- `include/map.h`
- `src/world/map/map_tiled_loader.c`
- `src/gameplay/task/task_seed_setup.c`
- `tests/map_layout_smoke.c`

---

### 已完成 5：Monsters 怪物出生点层接入

`world.tmj` 已新增 `Monsters` 对象层，当前有 8 个怪物出生点。

每个怪物出生点包含属性：

- `monsterType`
- `monsterName`
- `unlockStage`
- `gridX`
- `gridY`
- `sourceIndex`

`GameMap` 已新增：

- `hasMonsterLayer`
- `monsterSeeds[MAX_MONSTERS]`
- `monsterSeedCount`

任务初始化逻辑已改成：

1. 如果地图有 `Monsters` 层，优先使用地图怪物出生点
2. 如果地图缺失怪物层，回退旧的 `TasksContent_GetMonsterSeed(...)` 怪物表

当前测试覆盖：

- 怪物层成功加载
- 怪物数量与旧表一致
- final boss 坐标匹配 `BOSS_ARENA_BOSS_X/Y`
- relic guard 仍位于遗迹区域

相关文件：

- `include/map.h`
- `src/world/map/map_tiled_loader.c`
- `src/gameplay/task/task_seed_setup.c`
- `tests/map_layout_smoke.c`

---

### 已完成 6：Logs 日志点层接入

`world.tmj` 已新增 `Logs` 对象层，当前有 14 个日志点。

每个日志点包含属性：

- `sourceIndex`
- `titleKey`
- `logCategory`
- `logCategoryName`
- `gridX`
- `gridY`

`GameMap` 已新增：

- `hasLogLayer`
- `logSeeds[MAX_LOGS]`
- `logSeedCount`

任务初始化逻辑已改成：

1. 如果地图有 `Logs` 层，优先使用地图日志点的位置和类别
2. 日志正文仍通过 `sourceIndex` 从旧的 `TaskLogSeed` 表读取
3. 如果地图缺失日志层，回退旧的 `TasksContent_GetLogSeed(...)` 日志表

当前测试覆盖：

- 日志层成功加载
- 日志数量与旧表一致
- 第一条飞船日志位置匹配旧表
- 补充日志类别和位置匹配旧表
- 最后一条 Root Vault 日志位置匹配旧表

相关文件：

- `include/map.h`
- `src/world/map/map_tiled_loader.c`
- `src/gameplay/task/task_seed_setup.c`
- `tests/map_layout_smoke.c`

---

### 已完成 7a：第一批交互点锚点优先

已新增运行时锚点查询：

- `Map_GetRuntimeAnchorPosition(...)`
- `MapInternal_ActivateRuntimeAnchorLayer(...)`

`Map_Init` 现在会在地图加载成功或旧地图回退生成后，激活当前地图锚点，供没有直接持有 `GameMap` 的任务目标标记逻辑使用。

第一批已替换为“锚点优先，旧宏回退”：

- `COMM_RELAY`
- `CRASH_CLUE`
- `ENERGY_CORE`
- `SIGNAL_TOWER`

影响范围：

- 交互目标选择：`TasksRuntime_GetPreferredInteractionTarget(...)` 现在接收 `GameMap`，中继、残骸、信号塔会优先读取地图锚点
- 目标标记：`Tasks_GetObjectiveMarker(...)` 中相关路线目标会优先读取运行时锚点

当前仍保留旧坐标宏作为回退，也仍保留地图生成/默认锚点里的宏基准。

相关文件：

- `include/map.h`
- `src/world/map/map_internal.h`
- `src/world/map/map_layout.c`
- `src/world/map/map_runtime.c`
- `src/gameplay/task/task_interactions.cpp`
- `src/app/game_play_input.c`
- `src/gameplay/task/task_progress.c`
- `tests/map_layout_smoke.c`

---

### 已完成 7b：BOSS 点和石碑点锚点优先

第二批已替换为“锚点优先，旧宏回退”：

- `BOSS_SPAWN` / `BOSS_ARENA_BOSS_X/Y`
- `MONOLITH_A`
- `MONOLITH_B`
- `MONOLITH_C`

影响范围：

- 目标标记：`SetBossArenaMarker(...)` 的 BOSS 回退目标优先读取 `MAP_ANCHOR_BOSS_SPAWN`
- 目标标记：`SetNextMonolithMarker(...)` 的 A/B/C 石碑目标优先读取对应 `MAP_ANCHOR_MONOLITH_*`
- 交互目标选择：石碑 A/B/C 交互范围优先读取地图锚点
- 多格物体边界：中继、残骸、信号塔、石碑的运行时边界解析优先读取运行时锚点，Tiled 移动点位后交互/绘制边界能跟随

`BOSS_ARENA_PLAYER_ENTRY_X/Y` 当前没有需要替换的运行时玩法使用点，仍保留为默认锚点和测试基准。

阶段 7 当前状态：已按顺序完成交互点、BOSS 点和石碑点的运行时锚点优先接入。旧宏仍作为地图生成、默认锚点、旧表回退和测试期望基准保留。

相关文件：

- `src/world/map/map_layout_ship.c`
- `src/gameplay/task/task_interactions.cpp`
- `src/gameplay/task/task_progress.c`
- `tests/map_layout_smoke.c`

---

## 当前已知顺手修复

### 沼泽绳索屏障坐标重叠

之前三个沼泽绳索屏障中，有两个因为坐标缩放落在同一格。

已修改：

- `include/config.h`

从：

```c
#define ROPE_BARRIER_C_Y EXTERIOR_Y(60)
```

改为：

```c
#define ROPE_BARRIER_C_Y EXTERIOR_Y(61)
```

现在三个绳索屏障是独立格子。

---

## 当前验证状态

最近一次已运行：

```sh
make clean
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke
```

结果：全部通过。

如果切换对话后继续，应先重新确认：

```sh
git status --short
make smoke-map_layout
```

在大改动后再运行：

```sh
make clean
make smoke
```

---

## 当前未提交状态

地图数据化相关改动目前尚未提交。继续前建议先查看：

```sh
git status --short
```

当前预期会看到这些改动/新增：

- `CMakeLists.txt`
- `Makefile`
- `include/config.h`
- `include/map.h`
- `src/world/map/map_internal.h`
- `src/world/map/map_layout.c`
- `src/world/map/map_runtime.c`
- `src/gameplay/task/task_seed_setup.c`
- `tests/map_layout_smoke.c`
- `src/world/map/map_tiled_loader.c`
- `maps/`
- `docs/map_data_refactor_plan.md`

本阶段还会看到 `maps/world.tmj` 中新增 `Monsters` 和 `Logs` 对象层。

---

# 后续重构路线

## 阶段 5：Monsters 怪物出生点层（已完成）

### 目标

把当前硬编码怪物出生点迁移到 `world.tmj` 的 `Monsters` 对象层。

当前怪物数据来源：

- `src/content/task_content_data.cpp`
- `TaskMonsterSeed`
- `TasksContent_GetMonsterSeed(...)`
- `TasksRuntime_AddMonsterSpawn(...)`

### 建议新增地图层

在 `world.tmj` 中新增对象层：

```text
Monsters
```

每个对象属性：

- `monsterType`
- `monsterName`
- `unlockStage`
- `gridX`
- `gridY`
- `sourceIndex`

### 代码改造建议

在 `GameMap` 中新增：

```c
typedef struct MapMonsterSeed {
    int monsterType;
    int gridX;
    int gridY;
    int unlockStage;
} MapMonsterSeed;

bool hasMonsterLayer;
MapMonsterSeed monsterSeeds[MAX_MONSTERS 或固定数量];
int monsterSeedCount;
```

注意：需要先确认怪物最大数量常量。如果没有专用常量，可查看 `TaskSystem` 里的 monsters 数组容量。

修改 `src/world/map/map_tiled_loader.c`：

- 解析 `type = "monster"` 的对象
- 读取 `monsterType`、`unlockStage`
- 写入 `map->monsterSeeds`

修改 `src/gameplay/task/task_seed_setup.c`：

- 有 `map->hasMonsterLayer` 时优先使用地图怪物层
- 否则回退 `TasksContent_GetMonsterSeed(...)`

### 测试建议

在 `tests/map_layout_smoke.c` 或新增测试中确认：

- 怪物层成功加载
- 怪物数量与旧表一致
- final boss 坐标仍匹配 `BOSS_ARENA_BOSS_X/Y`
- relic guard 坐标仍在遗迹区域

再跑：

```sh
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke-endgame_new_survival
make smoke
```

---

## 阶段 6：Logs 日志点层（已完成）

### 目标

把飞船日志、主线日志、补充日志的位置迁移到 `world.tmj`。

当前日志数据来源：

- `src/content/task_content_data.cpp`
- `TaskLogSeed`
- `TasksContent_GetLogSeed(...)`

### 建议新增地图层

```text
Logs
```

每个对象属性：

- `logCategory`
- `titleKey` 或 `sourceIndex`
- `gridX`
- `gridY`
- `sourceIndex`

### 建议策略

第一步只迁移位置和类别，不迁移大段文本。

也就是说：

- 文本仍保留在 `task_content_data.cpp`
- 地图只控制日志出现位置
- 通过 `sourceIndex` 关联原日志文本

这样可以避免一次性把大量中英文剧情文本塞进 `world.tmj`。

---

## 阶段 7：逐步替换旧坐标宏使用点（已完成运行时接入）

### 目标

让更多玩法逻辑通过锚点读取位置，而不是直接使用坐标宏。

优先替换：

- `COMM_RELAY_X/Y`
- `CRASH_CLUE_X/Y`
- `ENERGY_CORE_NODE_X/Y`
- `SIGNAL_TOWER_X/Y`
- `BOSS_ARENA_BOSS_X/Y`
- `BOSS_ARENA_PLAYER_ENTRY_X/Y`
- `MONOLITH_A/B/C_X/Y`

### 注意

不要一次性替换所有宏。建议每次只替换一类：

1. 交互点
2. BOSS 点
3. 石碑点
4. 任务目标点

每替换一类就跑一次 smoke。

---

## 阶段 8：真正开始地图体验优化

等下面这些层都数据化后：

- Ground
- Props
- Area
- Hazard
- Anchors
- Resources
- Monsters
- Logs

就可以开始真正调整地图体验。

建议优化方向：

### 1. 基地作为安全中心

强化基地外出路线和回家路线，让玩家明确：

```text
基地 = 安全 / 恢复 / 整备 / 任务确认
```

### 2. 东侧沼泽风险层次

目标：

```text
外围提示风险
中层开始施压
深区高风险高收益
```

需要调整：

- `Hazard` 图层
- `Resources` 层
- `Monsters` 层
- 地图路径宽度

### 3. 西线调查感

强化：

- 船员踪迹
- 残骸
- 营地
- 调查路线

可以调整：

- `Anchors`
- `Logs`
- `Resources`
- `Props`

### 4. 南部设施压迫感

强化地下设施、下潜、真相揭露路线。

建议增加：

- 更清晰的设施入口
- 分段风险
- 关键回路/捷径

### 5. 北部终局仪式感

强化：

- 信号塔
- 石碑环
- BOSS 前场
- 最终选择路线

可以调整：

- `Props`
- `Area`
- `Anchors`
- `Monsters`


### 阶段 8 首轮完成记录（2026-07-08）

本阶段已从“继续底层迁移”切换为“直接优化地图体验”。首轮采用小步修改、每轮冒烟验证的方式完成了五个方向的第一批体验收尾：

1. **基地安全中心**
   - 清理气闸北侧肩部 `(84,48)` 的绊倒危险。
   - 目的：让基地外侧回家/出门边界更稳定地读作安全整备区。

2. **东侧沼泽风险层次**
   - 在通讯中继西侧增加安全踏脚带：`(89,48)`、`(90,48)`、`(91,48)`、`(92,48)`。
   - 将深沼入口 `(99,40)`、`(99,41)` 从毒危险降为普通沼泽危险。
   - 将入口侧低价值打捞点 `(103,50)` 降为普通沼泽危险。
   - 在绳索屏障旁增加准备站位 `(94,58)`。
   - 在 Crash Clue 西南侧增加中风险检查位 `(106,38)`。
   - 目的：形成“外围可读 → 中层施压 → 深区高风险高收益”的节奏。

3. **西线调查感**
   - 清理 West Frontier / Survey Break 日志连接处 `(40,70)`、`(39,71)` 的绊倒危险。
   - 目的：让调查节点之间有更安静的阅读停顿，而不是每一步都被随机绊倒打断。

4. **南部设施压迫感**
   - 在 Root Vault 前设施走廊 `(108,90)` 增加一处绊倒/故障警示。
   - 目的：让南部后段在不压住日志、资源或怪物点的前提下，有一个“设施正在失效”的压力提示。

5. **北部终局仪式感**
   - 将遗迹区域内残余森林缝 `(53,16)`、`(54,16)`、`(55,16)` 改为遗迹地面。
   - 目的：让石碑环北缘到信号塔平台视觉上更连续，更像同一片终局遗迹。

每轮改动后均已运行：

```sh
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke
```

结果均通过。

---

## 阶段 9：Tiled 编辑规则（已完成首版）

如果之后用 Tiled 打开：

```text
maps/world.tmj
```

请遵守这些规则。阶段 9 的目标不是继续改玩法，而是让后续人工或 AI 编辑地图时不破坏运行时读取约定。

### 1. 固定地图规格

- 不要改地图尺寸：`126 x 104`
- 不要改 tile 尺寸：`96 x 96`
- 不要改 tileset 的 `firstgid` 对应关系：
  - `spacecraft_tiles.tsj` 从 `1` 开始
  - `spacecraft_areas.tsj` 从 `101` 开始
  - `spacecraft_hazards.tsj` 从 `201` 开始

### 2. 不要删除或改名核心图层

必须保留这些图层名称：

- `Ground`
- `Props`
- `Area`
- `Hazard`
- `Anchors`
- `Resources`
- `Monsters`
- `Logs`

运行时按这些名字查找数据；改名会触发回退或导致对象层缺失。

### 3. 图层职责

- `Ground`：决定基础可走地面和地貌视觉。
- `Props`：决定障碍物、交互物、门、屏障等。大多数交互物/障碍会阻挡移动。
- `Area`：决定区域身份、氧气压力、地点名和任务分区。
- `Hazard`：决定绊倒、沼泽、毒危险等即时风险。
- `Anchors`：保存关键任务/路线点位。
- `Resources`：保存资源偏好点、产量、刷新次数和特殊标记。
- `Monsters`：保存怪物出生偏好点和解锁阶段。
- `Logs`：保存日志偏好点、类别和旧文本来源索引。

### 4. 对象层编辑规则

移动对象时，要同步：

- 对象自身的 `x` / `y`
- 对象属性里的 `gridX` / `gridY`

保持这些属性稳定：

- `sourceIndex` 不要重复、不要随意改。
- `resourceType`、`monsterType`、`logCategory` 使用现有枚举值。
- `titleKey`、`resourceName`、`monsterName`、`logCategoryName` 主要用于阅读和排查，建议与类型保持一致。

资源和日志对象可以放在“来源物体”或“目标物体”上；运行时会寻找附近合适的可走格生成实际节点。但体验优化时，关键目标旁边最好保留至少 1 个可读的可走站位。

### 5. 危险层编辑规则

- 基地周边优先保持无危险，作为安全中心。
- 外围路线可以少量使用 `HAZARD_TRIP` 提示不稳定。
- 沼泽外围优先使用 `HAZARD_SWAMP`。
- 深沼核心、高价值目标、深区怪物附近再使用 `HAZARD_POISON`。
- 不建议把所有相邻站位都画成最高危险；关键调查点最好保留 1 个低/中风险检查位。

### 6. 每次编辑后的验证顺序

小改动后至少运行：

```sh
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke
```

如果改了构建文件、加载器、坐标宏或大量地图数据，再额外运行：

```sh
make clean
make smoke
```

### 7. 当前首版规则验证状态

阶段 9 首版规则已经同步到：

- `docs/map_data_refactor_plan.md`
- `maps/README.md`

并在阶段 8 首轮体验优化后完成完整 smoke 验证。

---

## 切换对话后的继续提示词

如果切换新对话，可以直接对 Codex 说：

```text
请继续 /Users/jason/Documents/SpaceCraftLiving 的地图数据化重构。
先阅读 docs/map_data_refactor_plan.md，确认当前 git 状态和测试状态。
上次已经完成 Ground/Props/Area/Hazard/Anchors/Resources/Monsters/Logs 层接入，并完成阶段 7 的运行时锚点优先替换：COMM_RELAY、CRASH_CLUE、ENERGY_CORE、SIGNAL_TOWER、BOSS_SPAWN、MONOLITH_A/B/C。
当前已完成阶段 8 首轮地图体验优化和阶段 9 Tiled 编辑规则首版。下一步可以继续做更细的地图体验迭代，例如完整试玩东线/西线/南线/北线，或开始把更多旧坐标宏替换为 Anchors 查询。每次调整后跑 make smoke-map_layout、make smoke-task_interaction、make smoke-endgame_flow、make smoke。
```

---

## 当前建议

当前阶段 8 和阶段 9 已完成首版：

```text
阶段 8：首轮地图体验优化完成
阶段 9：Tiled 编辑规则首版完成
```

后续建议继续以“小步地图体验迭代 + 完整 smoke”的方式推进，优先通过实机路线检查验证东线、 西线、南线和北线的节奏。

---

## 方向修正：优先完成 Tiled 源文件化

用户当前目标已调整：先不继续地图体验优化，而是优先把现有地图内容进一步迁移到 Tiled，使 `maps/world.tmj` 更接近地图源文件，方便后续扩大地图、重做结构和替换美术瓦片。

新增详细计划：

- `docs/tiled_source_of_truth_plan.md`

当前下一步建议：

1. 新增 `Regions` 对象层，保存地点名矩形。
2. 让 `Map_GetLocationNameAt` 优先读取 Tiled Regions。
3. 新增或整理 `Unlocks` / 屏障数据，减少 gate、rope、boss entry 等硬编码坐标。
4. 暂时保持地图尺寸 `126 x 104`，不要马上改大。
5. 等 Regions / Unlocks 数据化后，再做动态地图尺寸支持。


# 2026-07-16 最终路线状态

地图数据化路线 A-D 已完成：Decor 支持、运行时动态尺寸、MiniMap 动态存储、V15 地图尺寸存档以及 Tiled 唯一正式源均已落地。旧文档中“保持 126 x 104”“加载失败回退旧生成器”等内容仅保留为历史阶段记录；当前规则以 `maps/README.md` 和 `docs/tiled_source_of_truth_plan.md` 的最终状态为准。

---

## 多地图拆分衔接记录（2026-07-19）

原 `world.tmj` 数据化成果已冻结并复制为 `legacy_world.tmj`。第一轮多地图拆分生成 `ship_interior.tmj` 与 `planet_surface_legacy.tmj`，但为保护旧坐标回退、复合物件边界、任务点和存档 V15，`ship_interior.tmj` 暂时保持 `126 × 104` 原坐标，不做裁剪和平移。

在地图切换和多地图存档完成前，完整玩法临时默认加载 `legacy_world`；两张拆分地图通过目录和专项 smoke 独立校验。后续地图数据迁移只能继续修改 `.tmj`，不能把拆分结果重新硬编码回 C/C++。
