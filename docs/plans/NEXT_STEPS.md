# SpaceCraft Living 阶段开发执行清单（2026-04-11）

## 当前统一口径

从当前这轮开始，项目执行口径固定为：

- `当前主目标是补齐西线 Echo Wilds 与南线 Subsurface Sink`
- `补齐西线 / 南线后，再冻结地图、剧情和玩法设计范围`
- `在此之前，不再继续扩展超出西线 / 南线补完之外的新地图、新剧情主线或大型新玩法系统`

如果后续有新增工作，也应优先归入下面几类：

- 西线补完
- 南线补完
- 主线整合
- 平衡调整
- 兼容清理
- 文档与测试护栏

当前执行快照（按代码与运行时口径）：

- `W1-W3`、`S1-S3` 已可跑通并已接入主流程
- 当前最优先缺口为 `W4-W5`、`S4-S5` 与 `X1-X3` 终局反哺整合
- 核心游玩体验已经固定为 `求生 + 搜集真相 + 主动选择结局`
- 大多数日志应继续通过任务、设施交互与调查闭环解锁，避免回退到“随手捡 lore”的旧结构
- 本文档中的“首批开工”段落改为历史快照，仅用于回归核对

## 当前最值得直接推进的任务（第二轮）

### 任务 1：西线任务链与区域落地

目标：

- 把 `Echo Wilds` 从首批可玩推进到完整调查线

本轮至少要完成：

- 按 [`../design/EXPANSION_MAP_DESIGN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_MAP_DESIGN.md) 固定 `West Frontier -> Survey Break -> Canopy Hollow -> Echo Basin -> Last Camp`
- 按 [`../design/EXPANSION_STORY_TASKS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_STORY_TASKS.md) 落地 `W1 - W5`
- 按 [`../design/WEST_SOUTH_FULL_STORY.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md) 对齐西线剧情事件与回基地总结口径
- 让西线承担失踪船员调查、人物痕迹补完与定居路线补强职责
- 让西线完成后能反哺地图情报、日志、基地总结和结局文本

### 任务 2：南线任务链与区域落地

目标：

- 把 `Subsurface Sink` 从首批可玩推进到完整设施与真相线

本轮至少要完成：

- 固定 `South Collapse -> Vent Galleries -> Service Shafts -> Purifier Ring -> Root Vault`
- 落地 `S1 - S5`
- 按 [`../design/WEST_SOUTH_FULL_STORY.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md) 对齐南线剧情事件与真相揭示口径
- 让南线承担设施控制、风险改写、事故真相与终局准备补强职责
- 让南线完成后能反哺主地图路线质量、设施收益和终局执行稳定性

### 任务 3：地图-剧情联动补齐

目标：

- 把当前地图做完整，并让基地、东线、北线、西线和南线都承担清晰剧情职责

本轮至少要完成：

- 按 [`MAP_STORY_ALIGNMENT_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/MAP_STORY_ALIGNMENT_PLAN.md) 固定各区域的叙事身份
- 补清楚 `进入理由 -> 信息收获 -> 返回结果` 这套区域闭环
- 统一 objective、communicator、field note、map overlay 与 objective marker 的口径
- 把西线 / 南线正式接入 `Stage 4 - Stage 7`，而不是悬浮在主流程外
- 修改顺序先按 `基地 -> 东线 -> 西线`，再进入 `基地 -> 南线 -> 北线 -> 终局选择`

### 任务 4：资源经济与终局预算收口

目标：

- 让关键资源需求更透明、更容易被玩家理解

本轮至少要完成：

- 明确 `Relic Fragment`、`Energy Core` 的最终预算口径
- 校对和平路线与英雄路线的准备成本说明
- 把关键预算同步到任务文本、通讯器提示和相关文档
- 校对 `Stage 1 - Stage 2` 的关键资源可达性，禁止出现“资源缺失导致主线无法推进”的硬锁
- 固定刷新边界：`飞船内资源只可拾取一遍`，`飞船外资源` 才进入刷新规则与调参范围
- 统一玩家侧刷新提示：只显示 `可采集 / 枯竭中 / 已恢复`，不显示刷新秒数和剩余次数

### 任务 5：战斗曲线与路线压力微调

目标：

- 让中后期战斗与终局路线风险更稳定、更可预判，同时保持“战斗不是核心推进方式”

本轮至少要完成：

- 固定普通怪物的“区域唯一威胁”定位，避免重新膨胀回刷怪型地图压力
- 微调中后期怪物与 Boss 的目标击杀轮次带
- 校对 Protection Suit、Field Camp、补氧道具和过滤类资源的实际收益
- 避免任何区域同时叠加过高直伤、过高耗氧、过高状态压力和过长跑路

### 任务 6：结局与菜单收尾

目标：

- 让游戏从“完整可玩”进一步靠近“完整发布体验”

本轮至少要完成：

- 固定 `飞船 / Loxi -> 结局分支点 -> 路线执行` 的终局顺序，并清除所有旧的“直接塔顶选结局”残留文案
- 把 `settlement` 的确认语义、两条救援路线关闭语义和路线复盘信息写清楚
- 补清楚结局后的返回路径
- 明确 `返回菜单 / 再开新局 / 读取存档` 的最小闭环
- 提高结局页的信息层次和路线复盘感

### 任务 7：兼容字段与旧语义继续收缩

目标：

- 把旧 `stamina / pressure / poison` 语义尽量压缩到兼容层

本轮至少要完成：

- 清点仍直接依赖旧字段的运行时入口
- 继续收敛旧命名、旧提示和旧判定语义
- 保证旧兼容逻辑主要停留在存档映射与底层壳层

### 任务 8：测试与文档护栏同步

目标：

- 防止后续调参把已成立的体验又改坏

本轮至少要完成：

- 为关键资源预算、刷新规则和典型战斗曲线补 smoke 护栏
- 同步更新活动文档与资源文档
- 让测试与文档都围绕“最终定版内容”而不是“扩展方向”写

## 推荐开发顺序表

为了避免西线、南线、北线和终局整合互相抢资源，当前建议按下面顺序推进：

### 阶段 A：西线第一轮补完（已完成基线）

目标：

- 先把 `Echo Wilds` 做成一条完整可验证的调查线

本阶段建议范围：

- 区域：`West Frontier`、`Survey Break`、`Canopy Hollow`
- 任务：`W1`、`W2`、`W3`
- 联动：基地出发理由、回基地总结、西线基础地图命名

本阶段完成判断：

- 玩家能正式进入西线
- 玩家能完成第一轮船员追踪
- 玩家能拿到西线第一批调查结论并回到基地结算

### 阶段 B：南线第一轮补完（已完成基线）

目标：

- 先把 `Subsurface Sink` 做成一条完整可验证的设施线

本阶段建议范围：

- 区域：`South Collapse`、`Vent Galleries`、`Service Shafts`
- 任务：`S1`、`S2`、`S3`
- 联动：基地扫描入口、南线第一批设施收益、主地图捷径或恢复点收益

本阶段完成判断：

- 玩家能正式进入南线
- 玩家能完成第一轮设施控制
- 玩家能感知南线收益已经回流到主地图

### 阶段 C：西线第二轮深化

目标：

- 把西线从“可玩”推进到“完整”

本阶段建议范围：

- 区域：`Echo Basin`、`Last Camp`
- 任务：`W4`、`W5`
- 联动：Loxi 分析更新、日志归档、定居路线与和平路线文本反哺

本阶段完成判断：

- 西线完成完整情感收束
- 玩家能明确理解失踪船员分流与留下理由
- 结局文本已有西线差异化依据

### 阶段 D：南线第二轮深化

目标：

- 把南线从“可玩”推进到“完整”

本阶段建议范围：

- 区域：`Purifier Ring`、`Root Vault`
- 任务：`S4`、`S5`
- 联动：区域风险改写、终局准备收益、系统级真相输出

本阶段完成判断：

- 南线完成完整真相收束
- 玩家能感知主地图风险结构被真实改变
- 终局路线已有南线差异化准备收益

### 阶段 E：跨区整合与终局反哺

目标：

- 把西线、南线、东线、北线收束成同一局游戏里的完整链路

本阶段建议范围：

- 任务：`X1`、`X2`、`X3`
- 北线：`Ruins Approach`、`Monolith Ring`、`Signal Tower Plateau`
- 结局：英雄、和平、定居三路线差异化文本与执行反馈

本阶段完成判断：

- 西线和南线不再是孤立内容包
- 玩家能在终局前明确感知“我之前在西线 / 南线做的事改变了这次终局”
- 三条成功路线都能引用西线 / 南线成果

### 阶段 F：发布前收口

目标：

- 把“内容补完”推进到“可发布闭环”

本阶段建议范围：

- 结局页和返回菜单
- 测试护栏
- 平衡微调
- 文档定稿

本阶段完成判断：

- 西线 / 南线 / 北线 / 结局全链路可跑
- 文档、测试、内容命名完全同步
- 达到“补齐后冻结”的版本要求

## 实现拆分建议

为了让每一轮开发都能稳定提交，建议每个阶段都拆成下面五类工作项：

地图内容：

- 子区域地标
- 路线组织
- 回撤点 / 捷径 / 风险点

交互内容：

- 任务触发
- 关键读取点 / 控制点
- 回基地结算点

文本内容：

- objective
- communicator
- field note
- 日志与结局引用文本

系统反哺：

- 主地图收益
- 终局提示更新
- 结局差异化依据

测试护栏：

- 任务触发顺序
- 关键状态切换
- 文本更新条件
- 结局分支引用

## 第一批已完成清单（历史快照）

下面这份清单用于记录首批已经落地的内容，主要用于回归核对：

- 西线：`W1 - W3`
- 南线：`S1 - S3`

该批次目标已经达成：两条线均已进入“可进入、可推进、可回基地结算”的第一轮可玩状态。

更细的文件级实施要求（历史执行快照），见 [`../archive/2026-04-11/plans/WEST_SOUTH_BATCH1_IMPLEMENTATION.md`](/Users/jason/Documents/SpaceCraftLivng/docs/archive/2026-04-11/plans/WEST_SOUTH_BATCH1_IMPLEMENTATION.md)。

### 第一批执行顺序（回放）

首批当时按下面顺序推进：

1. 先做西线入口与 `W1`
2. 再做西线中段与 `W2 - W3`
3. 再做南线入口与 `S1`
4. 再做南线中段与 `S2 - S3`
5. 最后统一基地总结、地图命名、测试护栏

这样做的原因：

- 西线和南线都先从“能进、能出、能结算”做起
- 避免一开始就把终局反哺和高阶真相混进首批实现
- 让首批版本先验证两条线是否真的成立

### A. 西线 `W1 - W3` 执行清单回放

#### 地图与运行时

当时优先修改：

- `src/map_layout_world.c`
  添加或接通 `West Frontier`、`Survey Break`、`Canopy Hollow` 的可进入结构
- `src/map_runtime.c`
  配置西线区域命名、进入判定、节点归属
- `src/game_runtime.cpp`
  添加进入西线时的即时提示和阶段反馈
- 西线相关渲染文件
  补第一批地标、残骸、观察营地与回撤路径表现

本步完成判断：

- 玩家能进入西线三段区域
- minimap / map overlay 能区分这三段

#### 任务与交互

当时优先修改：

- `src/task_content.cpp`
  增加 `W1 - W3` objective、field note、日志文案
- `src/task_progress.c`
  加入 `W1 -> W2 -> W3` 的推进条件
- `src/task_world_interactions.cpp`
  增加短报文点、痕迹点、调查节点、观察营地交互
- `src/task_ship_interactions.cpp`
  增加基地里与西线相关的出发理由和回收总结

本步完成判断：

- `W1 - W3` 可完整触发和推进
- 回基地后能拿到至少一次有效结算

#### UI 与叙事表达

当时优先修改：

- `src/ui_map_panel.cpp`
  展示西线命名与调查区身份
- `src/minimap.cpp`
  同步 `West Frontier / Survey Break / Canopy Hollow`
- `src/ui_log_reader.cpp`
  展示西线第一批日志

本步完成判断：

- 玩家在 HUD、地图、日志里看到的是同一套西线语义

#### 测试护栏

当时优先修改：

- `tests/task_text_smoke.c`
  覆盖 `W1 - W3` 文本入口
- 新增西线 smoke 测试
  覆盖触发、推进、回基地总结

本步完成判断：

- `W1 - W3` 的关键状态不会因后续调文案或判定被破坏

### B. 南线 `S1 - S3` 执行清单回放

#### 地图与运行时

当时优先修改：

- `src/map_layout_world.c`
  添加或接通 `South Collapse`、`Vent Galleries`、`Service Shafts`
- `src/map_runtime.c`
  配置南线区域命名、进入判定、节点归属
- `src/game_runtime.cpp`
  添加进入南线时的即时提示和设施反馈
- 南线相关渲染文件
  补第一批半埋设施、通风节点、维修井与捷径表现

本步完成判断：

- 玩家能进入南线三段区域
- minimap / map overlay 能区分这三段

#### 任务与交互

当时优先修改：

- `src/task_content.cpp`
  增加 `S1 - S3` objective、field note、设施记录文案
- `src/task_progress.c`
  加入 `S1 -> S2 -> S3` 的推进条件
- `src/task_world_interactions.cpp`
  增加扫描点、通风节点、维修井回路交互
- `src/task_ship_interactions.cpp`
  增加基地扫描、南线出发理由和回流收益总结

本步完成判断：

- `S1 - S3` 可完整触发和推进
- 完成后主地图至少有一项可感知收益

#### UI 与叙事表达

当时优先修改：

- `src/ui_map_panel.cpp`
  展示南线命名与设施区身份
- `src/minimap.cpp`
  同步 `South Collapse / Vent Galleries / Service Shafts`
- `src/ui_log_reader.cpp`
  展示南线第一批设施记录

本步完成判断：

- 玩家在 HUD、地图、日志里看到的是同一套南线语义

#### 测试护栏

当时优先修改：

- `tests/task_text_smoke.c`
  覆盖 `S1 - S3` 文本入口
- 新增南线 smoke 测试
  覆盖触发、推进、设施收益回流

本步完成判断：

- `S1 - S3` 的关键状态不会因后续调文案或判定被破坏

### C. 第一批统一收口

在 `W1 - W3` 与 `S1 - S3` 都可跑之后，再统一做下面这些：

- `src/task_content.cpp`
  统一西线 / 南线命名、objective 语气和基地总结口径
- `src/ui_map_panel.cpp`
  检查地图是否把西线 / 南线读成正式可玩区，而不是边界
- `src/minimap.cpp`
  检查 location 命名是否一致
- `tests/game_session_smoke.c`
  追加首批流程串联验证

完成标准：

- 玩家能完成西线第一轮内容
- 玩家能完成南线第一轮内容
- 两条线都能回基地结算
- 两条线都还没有和终局整合打架

## 未来预留位

当前项目不准备做账号系统，但允许保留一个很小的未来接口位。

可保留的唯一明确非当前范围预留是：

- `注册 / 登录 / 账号绑定`

这类能力当前不应进入 P0，也不应影响本地单机闭环。

如果未来真的要做，必须遵守：

- 默认仍支持本地离线游玩
- 账号层不能阻塞游戏启动
- 账号层不能重写当前本地存档主路径
- 账号相关实现必须独立成新模块，而不是塞进现有 gameplay / save / UI 大文件

详细约束见 [`../engineering/FUTURE_RESERVE.md`](/Users/jason/Documents/SpaceCraftLivng/docs/engineering/FUTURE_RESERVE.md)。

## 每轮开发固定流程

建议每一轮都按下面顺序推进：

1. 先看 [`../design/GAME_DESIGN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/GAME_DESIGN.md)
2. 再看 [`../design/CURRENT_STATUS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/CURRENT_STATUS.md)
3. 然后先对照 [`MAP_STORY_ALIGNMENT_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/MAP_STORY_ALIGNMENT_PLAN.md)
4. 再对照 [`BALANCE_TUNING_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/BALANCE_TUNING_PLAN.md)
5. 只改一个明确主题
6. 构建并跑 smoke 测试
7. 最后同步更新对应文档

## 当前版本里程碑建议

建议把后续版本按下面方式管理，而不是零散穿插提交：

### `M1 西线首通`

- 包含 `W1 - W3`
- 目标是让西线第一次真正可玩

### `M2 南线首通`

- 包含 `S1 - S3`
- 目标是让南线第一次真正可玩

### `M3 西线完结`

- 包含 `W4 - W5`
- 目标是完成西线完整情感线

### `M4 南线完结`

- 包含 `S4 - S5`
- 目标是完成南线完整设施线与真相线

### `M5 终局整合`

- 包含 `X1 - X3` 与结局反哺
- 目标是让西线 / 南线收益真正进入三路线

### `M6 冻结收口`

- 包含平衡、测试、菜单、文档定稿
- 目标是达到“补齐西线 / 南线后冻结”的最终要求

## 当前不建议优先做的事

- 再开超出西线 / 南线补完之外的新地图扩展设计
- 再写超出当前西线 / 南线目标之外的新剧情总纲
- 为了“内容更多”而继续加与当前任务链无关的区域面积
- 大量新增怪物、状态或路线系统
- 把未来账号功能提前塞进当前开发主线

## 本轮完成判断

当下面几项都满足时，可认为当前一轮工作完成：

- 西线或南线比改动前更接近完整任务链闭环
- 关键资源预算比改动前更清楚
- 战斗与路线压力比改动前更稳定
- 结局与菜单闭环比改动前更完整
- 兼容字段比改动前更薄
- 测试与文档已经同步
