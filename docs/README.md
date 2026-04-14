# SpaceCraft Living 文档入口

快速入口：

- [`ACTIVE_SET.md`](ACTIVE_SET.md)
  下一步开发必读集（仅保留当前阶段直接需要的文档）。

当前文档体系只保留三种角色：`活动文档`、`工程文档`、`历史参考`。

统一开发口径：

- 当前目标是在补齐 `西线 Echo Wilds` 与 `南线 Subsurface Sink` 内容后冻结地图与剧情范围
- 在此之前，不再继续扩展超出西线 / 南线补完之外的新地图、新剧情主线或大型新玩法系统
- 核心游玩体验固定为 `求生 + 搜集真相 + 主动选择结局`，而不是以连续战斗清场为主
- 主线剧情推进固定以 `任务 -> 日志 / 调查回收 -> 回船总结 -> 终局分支确认` 为骨架
- 活动开发围绕 `西线补完`、`南线补完`、`主线整合`、`平衡性`、`稳定性` 与 `发布级收尾` 展开

## 当前最值得先看

- [`design/GAME_DESIGN.md`](design/GAME_DESIGN.md)
  当前设计基线，定义固定玩法、世界布局、日志驱动推进与结局结构。
- [`design/CURRENT_STATUS.md`](design/CURRENT_STATUS.md)
  当前代码真实落地状态，回答“已经做到哪里”，并核对日志解锁、Loxi 分支点与战斗轻量化是否已落地。
- [`design/EXPANSION_MAP_DESIGN.md`](design/EXPANSION_MAP_DESIGN.md)
  西线 `Echo Wilds` 与南线 `Subsurface Sink` 的当前活动地图设计文档。
- [`design/EXPANSION_STORY_TASKS.md`](design/EXPANSION_STORY_TASKS.md)
  西线 / 南线任务链与结局反哺的当前活动剧情设计文档。
- [`design/WEST_SOUTH_FULL_STORY.md`](design/WEST_SOUTH_FULL_STORY.md)
  西线 / 南线完整剧情稿，覆盖五段西线调查、五段南线设施线与三段跨区联动的完整叙事流程。
- [`design/WEST_SOUTH_RESOURCE_GAP.md`](design/WEST_SOUTH_RESOURCE_GAP.md)
  西线 / 南线补完所需新增图片与视频资源清单。
- [`plans/BALANCE_TUNING_PLAN.md`](plans/BALANCE_TUNING_PLAN.md)
  当前主执行计划之一，聚焦数值、路线压力和测试护栏。
- [`plans/NEXT_STEPS.md`](plans/NEXT_STEPS.md)
  当前阶段（第二轮深化与终局整合）最适合直接执行的开发清单。

## 活动文档

- [`design/STATUS_SYSTEM_MAPPING.md`](design/STATUS_SYSTEM_MAPPING.md)
  状态系统映射表，说明区域、怪物、道具、装备和设施如何接入当前状态系统。
- [`design/EXPANSION_MAP_DESIGN.md`](design/EXPANSION_MAP_DESIGN.md)
  当前西线 / 南线地图补完设计，用于固定空间职责、路线结构与主地图反哺关系。
- [`design/EXPANSION_STORY_TASKS.md`](design/EXPANSION_STORY_TASKS.md)
  当前西线 / 南线剧情任务设计，用于固定两条调查链和跨区联动任务的推进结构。
- [`design/WEST_SOUTH_FULL_STORY.md`](design/WEST_SOUTH_FULL_STORY.md)
  西线 / 南线完整剧情正文，用于统一任务推进时的剧情事件与结局反哺语义。
- [`design/WEST_SOUTH_RESOURCE_GAP.md`](design/WEST_SOUTH_RESOURCE_GAP.md)
  西线 / 南线资源缺口清单，用于统一图片和视频资产新增优先级。
- [`plans/MAP_STORY_ALIGNMENT_PLAN.md`](plans/MAP_STORY_ALIGNMENT_PLAN.md)
  地图与剧情联动补齐执行单，用来固定基地、东线、北线、西线与南线职责。

## 工程文档

- [`engineering/ARCHITECTURE.md`](engineering/ARCHITECTURE.md)
  模块职责、工程边界和维护规则。
- [`engineering/CPP_MIGRATION_PLAN.md`](engineering/CPP_MIGRATION_PLAN.md)
  混合工程的语言边界与已有迁移策略，保留工程参考价值。
- [`engineering/Xcode_Setup.md`](engineering/Xcode_Setup.md)
  macOS / Xcode 的项目生成与运行方式。
- [`engineering/FUTURE_RESERVE.md`](engineering/FUTURE_RESERVE.md)
  未来可选能力预留，包含注册 / 登录接口边界。

## 资源文档

- [`../resources/README.md`](../resources/README.md)
- [`../resources/images/README.md`](../resources/images/README.md)
- [`../resources/audio/README.md`](../resources/audio/README.md)
- [`../resources/videos/README.md`](../resources/videos/README.md)

这些文档只描述“当前代码是否真的会加载对应资源”。

## 历史参考

下面这些内容保留背景价值，但不再指导当前实现：

- [`archive/2026-04-11/plans/MAP_RESTRUCTURE_PLAN.md`](archive/2026-04-11/plans/MAP_RESTRUCTURE_PLAN.md)
- [`archive/2026-04-11/plans/OPTIMIZATION_PLAN.md`](archive/2026-04-11/plans/OPTIMIZATION_PLAN.md)
- [`archive/2026-04-11/plans/SURVIVAL_SYSTEM_MIGRATION_PLAN.md`](archive/2026-04-11/plans/SURVIVAL_SYSTEM_MIGRATION_PLAN.md)
- [`archive/2026-04-11/plans/REFACTOR_COMPLETION_PLAN.md`](archive/2026-04-11/plans/REFACTOR_COMPLETION_PLAN.md)
- [`archive/2026-04-11/plans/WEST_SOUTH_BATCH1_IMPLEMENTATION.md`](archive/2026-04-11/plans/WEST_SOUTH_BATCH1_IMPLEMENTATION.md)
- [`archive/`](archive/)

## 使用原则

- 活动文档优先于历史参考。
- 如果文档和当前代码冲突，以当前代码为准。
- 如果活动文档之间有冲突，以 `GAME_DESIGN.md`、`CURRENT_STATUS.md`、`EXPANSION_MAP_DESIGN.md` 和 `NEXT_STEPS.md` 为准。
