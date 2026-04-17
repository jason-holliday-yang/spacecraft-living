# SpaceCraft Living 代码校准主线流程（2026-04-17）

## 1. 文档目的

本文档只服务一个目标：

- 把 `当前代码已经实现的主线流程` 按阶段拆开写清楚

它不是目标设计稿，也不是未来计划稿。  
如果它与其他活动文档冲突，以当前代码和本文档为准。

本轮核对基于以下实现文件：

- `src/task_content.cpp`
- `src/task_progress.c`
- `src/task_ship_interactions.cpp`
- `src/task_world_interactions.cpp`
- `src/task_crafting.cpp`
- `src/task_update.c`
- `src/task_content_data.cpp`
- `src/recipe_catalog.cpp`
- `include/config.h`

## 2. 固定交互点与关键坐标

### 2.1 飞船内固定交互点

| 功能 | 房间 | 坐标 | 说明 |
| --- | --- | --- | --- |
| 洛希终端 | `Terminal Bay` | `(57, 44)` | `Stage 6` 同步遗迹碎片，`Stage 7` 先复核档案，再正式选结局路线 |
| 氧气控制台 | `Life Support` | `(57, 58)` | `Stage 1`、`Stage 2` 的修复交互点；修复完成后也承担补满氧气的职责 |
| 工作台 | `Workshop` | `(69, 44)` | 高级制作固定在这里完成 |
| 气闸控制台 | `Airlock Link` | `(74, 48)` | `Stage 3` 用于打开外层路线；英雄路线会在这里传送进守卫战区 |
| 动力控制台 | `Power Bay` | `(69, 58)` | `Stage 5` 安装能源核心并修复动力 |

### 2.2 飞船内恢复规则

- `Crew Quarters` 休息会把生命值回满，并额外恢复一部分氧气、体力，降低中毒压力。
- `Life Support` 的氧气控制台会补满氧气并清掉低氧 / 窒息 / 漏氧等呼吸类异常。
- 当前代码不是“只要走进飞船就自动回满”。恢复是绑定在具体房间 / 交互点上的。

### 2.3 外部主线固定点

| 功能 | 区域 | 运行时坐标 | 设计锚点 | 说明 |
| --- | --- | --- | --- | --- |
| 通讯中继 | 东线外层 | `(93, 46)` | `EXTERIOR_X(100), EXTERIOR_Y(44)` | `Stage 3` 修复点 |
| 坠毁线索 | 东线深处 | `(101, 70)` | `EXTERIOR_X(110), EXTERIOR_Y(74)` | `Stage 4` 调查点 |
| 能源核心节点 | 深沼泽 | `(102, 68)` | `EXTERIOR_X(112), EXTERIOR_Y(72)` | `Stage 5` 的强制单点目标 |
| 遗迹入口指引点 | 北线入口 | `(64, 32)` | `EXTERIOR_X(64), EXTERIOR_Y(27)` | `Stage 6` 无碎片时的 objective marker |
| 信号塔 | 北线塔顶 | `(64, 17)` | `EXTERIOR_X(64), EXTERIOR_Y(8)` | 和平 / 英雄路线最终执行点 |

### 2.4 北线石碑与 Boss 战区

| 功能 | 运行时坐标 | 说明 |
| --- | --- | --- |
| 石碑 A | `(54, 23)` | 北线石碑解谜点 |
| 石碑 B | `(64, 20)` | 北线石碑解谜点 |
| 石碑 C | `(75, 24)` | 北线石碑解谜点 |
| Boss 战区入口 | `(81, 70)` | 英雄路线从气闸传送后的玩家落点 |
| Boss 本体 | `(86, 66)` | 独立 `guardian arena` 内的守卫位置 |
| Boss 战返回点 | `(78, 52)` | 守卫战后回到飞船气闸附近 |

说明：

- 英雄路线当前确实会把玩家传送进独立 `guardian arena` 区域。
- 但从代码实现上看，它仍然占用同一张运行时地图里的独立区域坐标，不能写成“当前代码里完全不存在于现有地图坐标系之外”。

## 3. 当前制作与阶段解锁规则

### 3.1 配方解锁规则

| 配方 | 何时可见 | 是否必须在工作台旁 | 额外条件 |
| --- | --- | --- | --- |
| `Glow Stick` | `Stage 2` | 否 | 无 |
| `Simple Rope` | `Stage 3` | 否 | 无 |
| `Field Camp` | `BASE_CAMP_UNLOCK_STAGE`，当前即 `Stage 3` | 否 | 无 |
| `Reinforced Metal` | `Stage 4` | 是 | 还要求状态稳定 |
| `Laser Gun` | `Stage 4` | 是 | 还要求状态稳定 |
| `Protection Suit` | `Stage 4` | 是 | 还要求状态稳定 |
| `Signal Amplifier` | 只有 `Stage 6` 在洛希终端同步完碎片后才可见 | 是 | 还要求状态稳定 |

### 3.2 关于 `N` 键界面

- `N` 对应的洛希面板不是开局就有。
- 第一次成功和洛希终端交互后，`communicatorUnlocked = true`，同时会解锁洛希房间门。
- 因此文档里如果要写 `N` 面板，必须写成“与洛希终端首次同步后可用”。

### 3.3 关于日志奖励

当前代码里，日志仍然不是纯叙事收集物。

- `TasksRuntime_GrantLogReward()` 仍会给：
- 最大生命值奖励
- 攻击力奖励
- 即时氧气恢复
- 压力缓解
- 资源奖励

因此任何把“当前版本日志已经完全移除数值奖励”写成既成事实的文档，都是不准确的。

## 4. 七阶段代码校准流程

## Stage 1: Wake Up / 苏醒

### 阶段目标

- 收集 `3 Wood`
- 收集 `2 Metal Scrap`
- 在飞船 `Life Support` 的氧气控制台完成第一次修复

### 完成位置

- 房间：`Life Support`
- 交互点：氧气控制台
- 坐标：`(57, 58)`

### Objective Marker 行为

- 固定指向氧气控制台 `OXYGEN_CONSOLE_X / OXYGEN_CONSOLE_Y`

### 完成条件与消耗

- 交互时消耗 `3 Wood + 2 Metal Scrap`

### 完成后实际解锁

- `oxygenRepairLevel = 1`
- 主线推进到 `Stage 2`
- 玩家立即恢复 `18` 点氧气
- `Glow Stick` 配方从这一阶段结束后变为可见

### 当前代码备注

- 这一阶段不会自动解锁 `N` 面板；`N` 的解锁仍取决于你是否真正与洛希终端交互。
- 气闸在 `Stage 3` 之前仍保持封闭。

## Stage 2: First Steps / 迈出第一步

### 阶段目标

- 获取 `Glow Moss`
- 获取 `Ore`
- 制作 `Glow Stick`
- 回到同一个氧气控制台完成第二次修复

### 完成位置

- 修复位置仍是 `Life Support` 氧气控制台
- 坐标：`(57, 58)`

### Objective Marker 行为

- 仍固定指向氧气控制台 `OXYGEN_CONSOLE_X / OXYGEN_CONSOLE_Y`

### 完成条件与消耗

- 玩家必须已经拥有 `Glow Stick`
- 交互时额外消耗 `1 Glow Moss + 1 Ore`

### 完成后实际解锁

- `oxygenRepairLevel = 2`
- 主线推进到 `Stage 3`
- 玩家立即恢复 `35` 点氧气
- 外层沼泽 / 气闸路线获得开放资格
- `Simple Rope` 与 `Field Camp` 配方会在后续阶段按规则可见

### 当前代码备注

- `Glow Stick` 属于基础配方，代码层没有强制要求一定要站在工作台旁制作。
- 文档里不要把它写成“必须在工作台制作”的硬条件。

## Stage 3: Into the Wild / 走向荒野

### 阶段目标

- 先在飞船使用气闸控制台打开外层路线
- 收集通讯修复所需材料
- 修复东线通讯中继

### 关键位置

| 节点 | 房间 / 区域 | 坐标 | 作用 |
| --- | --- | --- | --- |
| 气闸控制台 | `Airlock Link` | `(74, 48)` | 先打开外层路线 |
| 通讯中继 | 东线外层 | `(93, 46)` | 完成阶段修复 |

### Objective Marker 行为

- 玩家还在基地里时：指向气闸控制台 `(74, 48)`
- 玩家已经出到外部时：改为指向通讯中继 `(93, 46)`

### 完成条件与消耗

- 当前代码真正检查的材料是：
- `2 Alien Vine`
- `2 Shell Fruit`
- `1 Special Fungus`

### 完成后实际解锁

- `commRepairLevel = 1`
- 主线推进到 `Stage 4`
- 洛希能正确读取东线
- 西线调查线进入可回收状态
- 西线开放条件成立：`stage >= 4 && commRepairLevel >= 1`

### 当前代码备注

- 这里有一个很容易写错的点：阶段标题文本里写的是 `Vine / Fruit / Fungus`，但真正的代码消耗不是通用 `Plant Fruit`，而是 `Shell Fruit`。
- 所以开发文档必须写 `Shell Fruit`，不能只写泛称 `Fruit`。

## Stage 4: Rising Risk / 风险升级

### 阶段目标

- 制作 `Laser Gun`
- 制作 `Protection Suit`
- 前往东线残骸调查坠毁线索

### 关键位置

| 节点 | 房间 / 区域 | 坐标 | 作用 |
| --- | --- | --- | --- |
| 工作台 | `Workshop` | `(69, 44)` | 组装激光枪和防护服 |
| 坠毁线索点 | 东线深处 | `(101, 70)` | 完成阶段调查 |

### Objective Marker 行为

- 固定指向坠毁线索点 `(101, 70)`

### 完成条件与消耗

- `Laser Gun`：`1 Metal Scrap + 2 Ore`
- `Protection Suit`：`1 Wood + 2 Alien Vines + 1 Protective Fiber`
- 两个配方都要求：
- 玩家站在工作台旁
- 玩家状态稳定，能进行高级制作
- 线索点检查条件是：`hasLaserGun && hasProtectionSuit`

### 完成后实际解锁

- `crashClueFound = true`
- 主线推进到 `Stage 5`
- 深层沼泽入口获得开放资格
- 残骸黑匣子日志进入可回收状态

### 当前代码备注

- `Reinforced Metal` 配方在 `Stage 4` 也会可见，但它不是推进 `Stage 4` 的硬条件。
- 当前真正推进只检查“是否已经有激光枪和防护服”。

## Stage 5: Power Breakthrough / 动力突破

### 阶段目标

- 前往深沼泽取得 `Energy Core`
- 回飞船 `Power Bay` 安装动力模块

### 关键位置

| 节点 | 房间 / 区域 | 坐标 | 作用 |
| --- | --- | --- | --- |
| 能源核心节点 | 深沼泽 | `(102, 68)` | 当前代码保证会有且只需要 `1` 个 |
| 动力控制台 | `Power Bay` | `(69, 58)` | 安装能源核心并修复动力 |

### Objective Marker 行为

- 如果玩家已经拿到 `Energy Core`：指向 `Power Bay` 动力控制台 `(69, 58)`
- 如果玩家还没拿到 `Energy Core`：
- 优先指向当前活动中的能源核心节点
- 如果节点位置还没解析出来，则回退到深沼泽锚点 `(102, 68)`

### 完成条件与消耗

- `1 Energy Core`
- `2 Ore`
- `1 Energy Crystal`

### 完成后实际解锁

- `energyRepairLevel = 1`
- 主线推进到 `Stage 6`
- 北线遗迹入口开放
- 南线开放条件成立：`stage >= 5 && energyRepairLevel >= 1`

### 当前代码备注

- `TasksRuntime_EnsureEnergyCoreNode()` 会在 `Stage 5` 期间强制维护一个可拾取的能源核心节点。
- 只要玩家手里还没有能源核心，且动力还没修好，这个节点就会被保持为激活状态。
- 因此开发文档不要把能源核心写成“可能不刷”或“需要多点随机碰运气”的设计。

## Stage 6: Final Preparation / 最终准备

### 阶段目标

- 在北线遗迹收集 `3 Relic Fragments`
- 把整套碎片带回洛希终端同步

### 关键位置

| 节点 | 区域 | 坐标 | 说明 |
| --- | --- | --- | --- |
| 遗迹碎片 1 | 北线遗迹 | `(69, 25)` | `EXTERIOR_X(70), EXTERIOR_Y(18)` |
| 遗迹碎片 2 | 北线遗迹 | `(81, 20)` | `EXTERIOR_X(86), EXTERIOR_Y(12)` |
| 遗迹碎片 3 | 北线遗迹 | `(83, 25)` | `EXTERIOR_X(88), EXTERIOR_Y(18)` |
| 洛希终端 | `Terminal Bay` | `(57, 44)` | 完成同步 |

### Objective Marker 行为

- 玩家已经持有 `3` 枚遗迹碎片时：指向洛希终端 `(57, 44)`
- 尚未凑齐时：指向北线入口引导点 `(64, 32)`

### 完成条件与消耗

- 只检查玩家是否持有 `3 Relic Fragments`
- 在洛希终端交互后进入同步

### 完成后实际解锁

- `amplifierUnlocked = true`
- 主线推进到 `Stage 7`
- `Signal Amplifier` 配方开始可见

### 当前代码备注

- 这一步不会消耗遗迹碎片。
- 同步完成后，玩家手里的 `3 Relic Fragments` 仍然保留，可以在后续和平路线里继续拿去制作 `Signal Amplifier`。

## Stage 7: Final Choice / 最终抉择

### 阶段目标

- 收齐终局开放所需的主线档案
- 完成西线与南线最终调查闭环
- 回到洛希终端先复核档案，再正式选择路线

### 真正开放结局选择的代码条件

当前代码要求同时满足下面全部条件，`Tasks_IsEndingBranchReady()` 才会成立：

- `stage >= 7`
- `oxygenRepairLevel >= 2`
- `commRepairLevel >= 1`
- `energyRepairLevel >= 1`
- `crashClueFound == true`
- `amplifierUnlocked == true`
- `westW5Completed == true`
- `southS5Completed == true`
- `所有 Main Archive 日志都已回收`

在这之后还有一个额外步骤：

- 第一次在洛希终端交互，只会把 `endingArchiveReviewed = true`
- 第二次交互，才是真正进入路线确认

### Objective Marker 行为

| 当前状态 | Marker 指向 |
| --- | --- |
| 已满足结局开放条件，但还没选路线 | 洛希终端 `(57, 44)` |
| 已选英雄路线，Boss 未击败，且人还在正常地图 | 气闸控制台 `(74, 48)` |
| 已选英雄路线，且人已进入守卫战区 | Boss 位置 `(86, 66)` 或当前 Boss 实际坐标 |
| 已选英雄路线，Boss 已击败 | 信号塔 `(64, 17)` |
| 已选和平路线，且已做好 `Signal Amplifier` | 信号塔 `(64, 17)` |
| 已选和平路线，但手里只有碎片还没做放大器 | 工作台 `(69, 44)` |
| 已选和平路线，碎片都还没凑齐 | 北线入口引导点 `(64, 32)` |
| 还没达到结局开放条件 | 下一座未激活石碑 |

### 英雄路线

- 选择地点：洛希终端 `(57, 44)`
- 进入方式：去飞船气闸控制台 `(74, 48)` 交互
- 传送落点：`guardian arena` 入口 `(81, 70)`
- Boss 位置：`(86, 66)`
- 打完后返回信号塔 `(64, 17)` 完成最终发射

### 和平路线

- 选择地点：洛希终端 `(57, 44)`
- 还需要：
- `3 Relic Fragments`
- 在工作台旁、状态稳定时制作 `Signal Amplifier`
- 制作完成后把放大器带去信号塔 `(64, 17)`

### 定居路线

- 选择地点：洛希终端 `(57, 44)`
- 当前代码保持显式确认语义
- 它不是第一次碰到塔就会自动触发的旧写法

### 当前代码备注

- 英雄路线已经是“先在飞船选路线，再在气闸触发守卫战”的流程。
- 和平路线当前仍明确依赖 `Signal Amplifier`。
- `Settlement` 仍然需要主动确认，不能把它写成隐式结局。

## 5. 日志解锁顺序的代码事实

这部分是 `Stage 7` 文档最容易漏掉的前置条件，因此单独列出。

当前 `TasksRuntime_SyncLogAvailability()` 的日志解锁顺序如下：

| 日志索引 | 解锁条件 |
| --- | --- |
| `0`、`1`、`2` | 开局就可用 |
| `3` | `commRepairLevel >= 1` |
| `4` | `westW1Completed` |
| `5` | `westW2Completed` |
| `6` | `westW3Completed` |
| `7` | `westW4Completed` |
| `8` | `crashClueFound == true` |
| `9` | `energyRepairLevel >= 1` |
| `10` | `southS1Completed` |
| `11` | `southS2Completed` |
| `12` | `southS3Completed` |
| `13` | `southS4Completed` |

因此：

- `Stage 7` 不是单纯“到终局后去塔上选结局”
- 它实际要求玩家把主线修复、西线 / 南线推进、日志回收与洛希复核全部做完

## 6. 当前最容易写错的开发口径

后续其他文档如果要引用主线流程，至少要避开下面这些误写：

- 不要把 `Stage 3` 的材料写成泛称 `Fruit`；当前代码检查的是 `Shell Fruit`
- 不要把 `Glow Stick` 写成必须在工作台制作；当前代码没有这个硬限制
- 不要把 `Signal Amplifier` 写成 `Stage 6` 同步时就被自动做出来；当前只是解锁配方
- 不要把 `Relic Fragments` 写成在洛希同步时被消耗；当前不会消耗
- 不要把日志写成“当前版本已经纯叙事化”；当前仍保留数值 / 资源奖励
- 不要把英雄 Boss 战区写成“当前代码里完全脱离现有地图坐标系”；当前更准确的说法是“通过气闸传送进入的独立区域”
