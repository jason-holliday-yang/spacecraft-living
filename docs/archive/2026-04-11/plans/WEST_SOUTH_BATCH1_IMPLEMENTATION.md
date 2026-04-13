# SpaceCraft Living 西线 / 南线首批实施单（2026-04-11）

> 执行状态快照：`已完成首批落地`
>
> 本文档当前角色是“执行回放与回归核对参考”，对应已落地基线：
>
> - 西线：`W1-W3`
> - 南线：`S1-S3`
>
> 当前主执行重心已切换到第二轮深化：`W4-W5`、`S4-S5` 与 `X1-X3` 终局整合。

## 1. 文档目的

本文档记录首批正式开工内容的执行基线：

- 西线 `W1 - W3`
- 南线 `S1 - S3`

它不负责定义长期方向，也不重复解释全部世界观。

它主要回答下面这些问题：

- 首批到底改了哪些内容
- 这些内容当时按什么顺序落地
- 每个文件在首批阶段承担了什么职责
- 这一批的完成标准是什么
- 这一批最容易出错的地方是什么

阅读顺序建议：

1. [`../design/GAME_DESIGN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/GAME_DESIGN.md)
2. [`../design/EXPANSION_MAP_DESIGN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_MAP_DESIGN.md)
3. [`../design/EXPANSION_STORY_TASKS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_STORY_TASKS.md)
4. [`../design/WEST_SOUTH_FULL_STORY.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md)
5. [`NEXT_STEPS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/NEXT_STEPS.md)
6. 本文档

## 2. 首批目标

首批不是要把西线 / 南线一次做完，而是要完成下面这件事：

- 让西线和南线都从“占位方向”变成“正式可进入、可推进、可回基地结算”的区域内容

首批必须同时做到：

- 西线可以完成 `W1 - W3`
- 南线可以完成 `S1 - S3`
- 两条线都有进入理由、区域收获、返回结果
- 两条线都已经对主地图或后续结局形成轻量反哺

首批明确不要求：

- 完成 `W4 - W5`
- 完成 `S4 - S5`
- 完成 `X1 - X3`
- 重写所有结局全文
- 做完所有终局反哺

## 3. 首批范围

### 3.1 西线范围

首批只覆盖：

- `West Frontier`
- `Survey Break`
- `Canopy Hollow`
- `W1`
- `W2`
- `W3`

### 3.2 南线范围

首批只覆盖：

- `South Collapse`
- `Vent Galleries`
- `Service Shafts`
- `S1`
- `S2`
- `S3`

### 3.3 必须同步的基础联动

- 基地出发理由
- 回基地总结
- 地图命名
- minimap 命名
- 第一批日志
- 第一批 smoke 护栏

## 4. 开发顺序

建议严格按下面顺序推进：

1. 西线入口与 `W1`
2. 西线中段与 `W2`
3. 西线第三段与 `W3`
4. 南线入口与 `S1`
5. 南线中段与 `S2`
6. 南线第三段与 `S3`
7. 基地总结、地图命名、日志归档统一
8. smoke 测试补齐

这样安排的原因：

- 先把一条线做通，比两条线同时半成更容易验证
- 西线更偏调查和文本，适合先验证任务链结构
- 南线更偏设施和收益，适合在西线结构验证后推进

## 5. 文件职责拆分

### 5.1 地图与运行时

#### `src/map_layout_world.c`

本批职责：

- 接通西线三段与南线三段的首批区域结构
- 确保区域之间存在合理往返路径
- 避免首批就引入过长空跑

本批不要承担：

- 第二轮深区
- 完整终局反哺逻辑

#### `src/map_runtime.c`

本批职责：

- 注册首批区域命名
- 配置进入判定与区域归属
- 确保任务节点与区域边界一致

本批不要承担：

- 大量跨区状态逻辑

#### `src/game_runtime.cpp`

本批职责：

- 进入西线 / 南线时给出即时反馈
- 在首批任务节点完成后刷新提示
- 支撑回基地前后的运行时状态变化

### 5.2 任务与交互

#### `src/task_content.cpp`

本批职责：

- 提供 `W1 - W3`、`S1 - S3` 的 objective、field note、日志文本
- 提供基地出发理由和回基地总结
- 提供首批轻量结局前置依据

优先级：

- 最高

#### `src/task_progress.c`

本批职责：

- 串起 `W1 -> W2 -> W3`
- 串起 `S1 -> S2 -> S3`
- 保证首批任务不会乱序推进

#### `src/task_world_interactions.cpp`

本批职责：

- 落调查点、扫描点、节点控制点、观察营地、维修井等关键交互
- 把“区域存在”转成“任务可推进”

#### `src/task_ship_interactions.cpp`

本批职责：

- 基地里补西线 / 南线出发理由
- 基地里补首批完成后的总结
- 让回基地真的形成闭环

### 5.3 UI 与叙事表达

#### `src/ui_map_panel.cpp`

本批职责：

- 正式把西线 / 南线画成可玩区，而不是边界提示
- 显示首批区域命名和阶段身份

#### `src/minimap.cpp`

本批职责：

- 同步首批区域 location 名称
- 避免和 map overlay 命名不一致

#### `src/ui_log_reader.cpp`

本批职责：

- 显示西线 / 南线首批日志
- 保证日志归档顺序与任务推进一致

### 5.4 测试

#### `tests/task_text_smoke.c`

本批职责：

- 覆盖西线 / 南线首批文本入口
- 覆盖任务完成后的文本更新

#### `tests/game_session_smoke.c`

本批职责：

- 覆盖首批任务接入主流程后的会话稳定性

#### 新增首批 smoke 测试

本批职责：

- 覆盖 `W1 - W3` 顺序推进
- 覆盖 `S1 - S3` 顺序推进
- 覆盖回基地总结

## 6. 西线首批实施清单

### 6.1 `W1 西向残响`

必须完成：

- 基地收到西侧异常短报文
- `West Frontier` 可进入
- 第一处痕迹检查点可交互
- 回基地后出现第一次调查总结

完成标准：

- 玩家知道西线现在已经是正式目标

### 6.2 `W2 断裂勘测线`

必须完成：

- `Survey Break` 可进入
- 两个调查节点可完成
- 调查结论能指向船员撤离路径
- `W2` 完成后能进入 `W3`

完成标准：

- 玩家知道西线玩法是追踪和调查，不是刷怪或刷资源

### 6.3 `W3 林冠静默`

必须完成：

- `Canopy Hollow` 可进入
- 观察营地可发现
- 中途支撑点可识别
- 回基地后能得到“长期停留痕迹”结论

完成标准：

- 玩家知道西线已经进入人物线，而不只是路线线

## 7. 南线首批实施清单

### 7.1 `S1 沉降裂口`

必须完成：

- 基地扫描能触发南线入口
- `South Collapse` 可进入
- 第一处设施残响可读取
- 回基地后能确认南线不是普通洞穴

完成标准：

- 玩家知道南线是设施线，不是地下换皮图

### 7.2 `S2 通风廊校准`

必须完成：

- `Vent Galleries` 可进入
- 第一批通风 / 供氧节点可控制
- 节点控制前后风险差异可感知
- 回基地后系统判断更新

完成标准：

- 玩家第一次感到“环境因为我的操作而改变”

### 7.3 `S3 维修井回路`

必须完成：

- `Service Shafts` 可进入
- 维修井主回路可接通
- 至少一个捷径或稳定收益可回流到主地图
- 回基地后有明确收益总结

完成标准：

- 玩家知道南线不是只给信息，还会真实改变主地图质量

## 8. 首批统一验收标准

首批完成后，必须同时满足下面条件：

1. 西线和南线都已经是正式可进入区域
2. `W1 - W3` 和 `S1 - S3` 都能独立跑通
3. 两条线都能回基地结算
4. map overlay、minimap、objective、communicator 命名一致
5. 首批日志能正常显示
6. 首批 smoke 测试能覆盖核心顺序
7. 现有 `Stage 1 - Stage 7` 主线没有被打断

## 9. 首批风险点

最容易出错的地方有：

- 西线 / 南线地图区域先做了，但任务没有正式接入
- task 内容先写了，但 map 和 minimap 仍把区域当边界
- 交互点存在，但回基地后没有总结，导致流程断掉
- 先做了太多第二轮内容，反而拖慢首批闭环
- 西线和南线命名不一致，导致 UI 和文本打架

对应处理原则：

- 先闭环，再扩内容
- 先可推进，再补表现
- 先首批稳定，再做第二轮深化

## 10. 与其他文档的关系

这份文档的定位是：

- 比 [`NEXT_STEPS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/NEXT_STEPS.md) 更具体
- 比 [`EXPANSION_STORY_TASKS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_STORY_TASKS.md) 更偏实施
- 比 [`MAP_STORY_ALIGNMENT_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/MAP_STORY_ALIGNMENT_PLAN.md) 更聚焦首批开工

如果几份文档出现冲突，首批实施阶段建议按下面优先级处理：

1. 本文档
2. [`NEXT_STEPS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/NEXT_STEPS.md)
3. [`EXPANSION_STORY_TASKS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_STORY_TASKS.md)
4. [`MAP_STORY_ALIGNMENT_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/MAP_STORY_ALIGNMENT_PLAN.md)
