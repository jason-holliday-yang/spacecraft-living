# SpaceCraft Living 重构收口说明（历史阶段总结）

## 说明

本文档不再作为“当前最优先执行计划”使用。

它的作用已经调整为：

- 记录上一轮结构性重构的收口目标
- 解释为什么当前工程会保留 `game_runtime`、多块 UI 切片与 task/runtime 拆分边界
- 提醒读者：这里描述的是一轮历史重构阶段的成果，不代表当前最新玩法迁移优先级

当前真正的活动优先级请改看：

- [`NEXT_STEPS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/NEXT_STEPS.md)
- [`OPTIMIZATION_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/archive/2026-04-11/plans/OPTIMIZATION_PLAN.md)
- [`CURRENT_STATUS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/CURRENT_STATUS.md)

## 这一轮重构当时的目标

在不改变当时玩法规则、控制方式、存档语义与内容推进的前提下，完成一轮结构性收口：

- coordinator 边界清晰
- 测试尽量链接真实运行时边界
- 重复规则收敛到单一来源
- 文档与代码结构重新对齐

## 这轮重构留下来的有效成果

以下成果仍然是当前工程可继续复用的资产：

- `game_runtime` 作为共享运行时 helper 层的方向是正确的
- UI 已经从巨型单文件拆到 `ui_layout / ui_menu / ui_status / ui_inventory / ui_overlays`
- 任务运行时更新已经从 `task_system.c` 抽离到 `task_update.c`
- 会话层 smoke 测试已经能覆盖更接近真实流程的开局、读档、恢复和失败路径

## 需要特别说明的历史偏差

上一轮文档里曾提前记录过一组更细粒度的拆分文件名，例如：

- `task_setup.c`
- `task_progress.c`
- `task_economy.c`
- `task_targeting.c`
- `map_runtime.c`
- `map_layout.c`

当时这些名字一度先出现在文档里、后追到代码里，造成过阶段性偏差。

当前仓库的真实状态是：

- `task_system.c` 已缩到基础空间判定与阻挡查询
- `task_setup.c / task_progress.c / task_economy.c / task_targeting.c / task_update.c / task_actions.cpp / task_interactions.cpp / task_crafting.cpp` 已形成更稳定的 task 分层
- `map_layout.c / map_runtime.c / map_render_ground.c / map_render_props.c / map.c` 已形成地图生成、运行时规则、渲染切片、绘制编排的分层

也就是说：

- 那一轮“继续切开”的方向最终被落实，但活动文档必须始终和当前代码同步
- 当前工程事实仍应以 [`ARCHITECTURE.md`](/Users/jason/Documents/SpaceCraftLivng/docs/engineering/ARCHITECTURE.md) 和代码目录为准

## 对当前阶段仍有价值的结论

这份历史收口说明仍然提供两个有效判断：

### 1. 共享 helper 层是值得保留的

像下面这些逻辑继续集中管理是合理的：

- HUD message 生命周期
- Story Scene 开关
- Audio Scene 选择
- 会话层与运行时之间的共享辅助逻辑

### 2. 文档必须跟着代码真实状态更新

这轮历史重构最大的问题不是“拆得不够”，而是某些文档停留在中间态，后来代码又继续收敛，导致活动文档和仓库真实结构出现偏差。

因此当前阶段必须坚持：

- 代码改了，活动文档同轮更新
- 中间态拆分计划不要写成最终事实
- 真实结构以活动工程文档和仓库文件树为准

## 这份文档当前的正确使用方式

应把它当作：

- 一份“为什么工程边界会长成现在这样”的背景说明

不应把它当作：

- 当前新玩法迁移的直接施工蓝图
- 当前文件结构的逐项事实清单

## 当前阶段的接力关系

如果继续推进项目，正确顺序应是：

1. 先看新生存系统设计文档
2. 再看当前代码状态审计
3. 再看近期执行清单
4. 最后仅在需要理解历史重构背景时回看本文档
