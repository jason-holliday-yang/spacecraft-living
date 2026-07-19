# 地图设计审计 v1

日期：2026-07-08

## 当前结论

地图已具备 Tiled 优化基础。`world.tmj` 已承载 Ground、Props、Area、Hazard、Anchors、Resources、Monsters、Logs，可直接开始关卡设计迭代。

## 世界结构

- 中央：飞船基地，安全/恢复/整备。
- 东侧：沼泽风险线，氧气/毒雾/高价值资源。
- 西侧：Echo Wilds，调查/船员踪迹/日志链。
- 南侧：设施线，真相揭示/Root Vault。
- 北侧：遗迹终局线，石碑/信号塔/Boss。

## 数据现状

| 区域 | 格数 | 可走格 | 危险特点 |
|---|---:|---:|---|
| Base | 817 | 297 | 无危险 |
| Forest/Echo | 3940 | 3540 | 少量 Trip，仍偏安全 |
| Swamp Outer | 463 | 449 | 以 Swamp 为主 |
| Swamp Deep | 450 | 419 | 以 Poison 为主 |
| Ruins/South | 1797 | 1582 | 基本安全，少量 Trip |
| Boss | 196 | 141 | 无危险 |

对象分布：

- Resources：Base 7，Forest/Echo 25，Swamp Outer 13，Swamp Deep 2，Ruins/South 22。
- Monsters：Forest/Echo 3，Swamp Deep 1，Ruins/South 3，Boss 1。
- Logs：Base 3，Forest/Echo 5，Swamp Deep 1，Ruins/South 5。
- Anchors：Base 8，Forest/Echo 2，Swamp Outer 4，Swamp Deep 4，Ruins/South 7，Boss 2。

## 主要问题

### 1. 西线空间偏大但叙事密度不足

Forest/Echo 可走格很多，资源多，日志链完整，但地标和路线表达还可以更强。

建议：强化 West Frontier → Survey Break → Canopy Hollow → Echo Basin → Last Camp 的“人走过的痕迹”。

### 2. 南线设施感还不够强

Ruins/South 区域很大，危险少，资源多。Root Vault 线有内容，但设施递进感可加强。

建议：用 Props/Ground/Hazard 做出 South Collapse → Vent Galleries → Service Shafts → Purifier Ring → Root Vault 的层级。

### 3. 北线终局仪式感仍可增强

Monolith Ring、Signal Tower、Boss 区已有结构，但过渡和视觉中心还可更集中。

建议：强化遗迹地面连续性、石碑周边留白、Signal Tower 前场。

### 4. 东线风险层次已改善，但可继续精修

Outer Swamp 到 Deep Swamp 已有过渡。后续重点应是资源诱惑和回撤路线，而不是继续加毒区。

### 5. 部分地点名仍由代码判断

大结构重排时不能只改 Tiled。地点名和路线逻辑仍涉及：

- `src/world/map/map_runtime.c`
- `src/app/session/game_route_runtime.cpp`
- `src/content/task_content.cpp`
- `src/gameplay/task/task_survival_support.c`

## 推荐下一步

优先顺序：

1. 西线调查路线重塑。
2. 南线设施递进重塑。
3. 北线终局前场重塑。
4. 东线只做精修。
5. 必要时再补小范围代码重构。

## 操作规则

每次只改一个区域的小目标。改完运行：

```sh
make smoke-map_layout
make smoke-task_interaction
make smoke-endgame_flow
make smoke
```

## 已执行优化记录

### 2026-07-08：西线调查路线第一批

目标：减少日志阅读点和 Last Camp 收束点附近的随机绊倒打断。

修改：

- 清理 Canopy Hollow / West Frontier / Survey Break 周边绊倒点：`(45,65)`、`(46,64)`、`(44,66)`、`(43,67)`、`(42,68)`、`(41,69)`、`(31,68)`。
- 清理 Last Camp 入口收束线：`(44,77)`、`(43,78)`、`(42,79)`、`(41,80)`。

结果：

- 西线日志链阅读更稳定。
- Last Camp 更像调查终点。
- 每轮均通过 `make smoke-map_layout`、`make smoke-task_interaction`、`make smoke-endgame_flow`、`make smoke`。
