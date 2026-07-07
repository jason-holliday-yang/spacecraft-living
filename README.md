# SpaceCraft Living

`SpaceCraft Living` 是一个基于 `raylib` 的原生单人生存探索游戏。当前代码库已经收口为混合架构：

- `C` 负责地图、玩家、任务更新等核心运行时
- `C++` 负责资源、UI、会话编排、持久化与内容组织

玩家在外星坠毁后醒来，需要在飞船、森林、孢子沼泽与遗迹之间求生、修复系统，并通过日志与调查任务持续拼合真相。终局不是单纯打穿战斗，而是在收齐关键线索后回到飞船，与 `Loxi` 一起确认自己要走向哪一种结局。

---

## 目录

1. [快速开始](#快速开始)
2. [游戏概述](#游戏概述)
3. [核心玩法](#核心玩法)
4. [操作指南](#操作指南)
5. [项目架构](#项目架构)
6. [当前状态](#当前状态)
7. [开发指南](#开发指南)
8. [资源说明](#资源说明)

---

## 快速开始

### 构建

使用 `Makefile`：

```sh
make
./build/SpaceCraftLiving
```

如果 `raylib` 不在默认路径：

```sh
make RAYLIB_PREFIX=/custom/prefix
```

使用 `CMake`：

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```

使用 `Xcode`：

```sh
cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=/opt/homebrew
open build/xcode/SpaceCraftLiving.xcodeproj
```

不要直接把仓库根目录或 `build/` 目录作为 Xcode 工程打开。

### 测试

运行全部 smoke 测试：

```sh
make smoke
```

运行单项 smoke：

```sh
make smoke-session
make smoke-save
make smoke-map
```

---

## 游戏概述

### 项目定位

`SpaceCraft Living` 是一款轻量级单人异星生存探索游戏，核心体验不是"硬核沙盒求生"，而是：

- 有明确目标的生存推进
- 有风险管理的区域探索
- 有持续拼合真相的调查过程
- 有资源准备的路线规划
- 有清晰提示且由玩家主动选择的多结局收束

### 世界布局

最终世界布局已固定为：

- **中央偏上**：飞船基地（安全区、修复中枢、工作台与洛希交互中心、完整恢复点）
- **北侧**：石碑、遗迹、Signal Tower 所在的终局主线区
- **东侧**：swamp 主风险区（承接外沼泽与深区内容）
- **西侧**：回响荒野（Echo Wilds）调查/绕行/船员踪迹区
- **南侧**：地下沉降带（Subsurface Sink）设施/下潜/真相揭示区

### 飞船结构

飞船内部采用 `中央主通道 + 上下两侧船舱` 布局：

- 中央主通道：承担飞船内部主路径与区域切换
- 上层终端舱：洛希终端、任务与日志归档、终局路线确认
- 上层医疗支援舱：异常缓解、临时过滤、剧情支援点
- 下层氧气舱：氧气控制台与基础恢复功能
- 下层制作舱：工作台、核心装备制作
- 侧向储藏舱：配方引导、物资说明、早期教学互动
- 气闸连接舱：外出前最后准备点与区域出口

---

## 核心玩法

### 核心生存系统

最终玩法基线已固定为 `Health + Oxygen + Status Bar`：

- **血量**：负责承伤与濒死判定
- **氧气**：负责外出时长、区域推进与路线风险
- **状态栏**：负责表达中毒、漏氧、低氧、窒息、过滤强化等持续状态

#### 状态系统

状态栏显示于左下角，位于血量条上方。支持以下核心状态：

- **中毒**：持续损失血量、提高氧气消耗
- **漏氧**：额外消耗氧气
- **低氧**：自动阈值警告，提示进入撤退窗口
- **窒息**：最致命的自动状态，持续损失血量
- **危急状态**：血量过低时自动触发的危险状态
- **过滤呼吸**：防护服、滤芯或特殊药剂带来的正面状态
- **氧气储备**：复苏口粮、应急供氧装置带来的正面状态
- **营地恢复**：营地短暂停留后获得的正面状态

#### 恢复层级

- **基地**：完整回血、完整补氧、完整清异常
- **营地**：部分回血、部分补氧、有限缓解异常
- **消耗品**：临时止损或换取继续推进的窗口

#### 死亡与重开规则

死亡采用单次失败制：生命值归零后本轮立即结束，只能选择重新开始或读档。从死亡后的"重新开始"进入时，会跳过开场坠毁引导剧情。

### 阶段式主线结构

项目采用 `Stage 1 - Stage 7` 的主线推进方式：

#### Stage 1：初始苏醒

- 熟悉移动与交互
- 采集木材与金属废料
- 修复下层氧气控制台第一段
- 让玩家理解"离开安全区就开始消耗氧气"

#### Stage 2：初步探索

- 获取发光苔与矿石
- 制作荧光棒
- 完成氧气系统修复
- 明确氧气修复的价值不是"删掉系统"，而是降低长期外出压力

#### Stage 3：深入异星

- 使用气闸控制台离开基地
- 采集异星藤蔓、植物果实、特殊菌株等中期资源
- 修复通讯中继
- 开始让玩家重视路线长度与氧气余量

#### Stage 4：危机四伏

- 使用工作台制作激光枪与防护服
- 调查坠毁线索
- 强化"装备改变探索资格"的概念

#### Stage 5：能源突破

- 取得能源核心
- 修复能源模块
- 开启深层沼泽与更高耗氧区

#### Stage 6：终极准备

- 收集 3 个遗迹碎片
- 返回上层终端与洛希同步终局路径
- 让玩家进入真正的长路线风险管理

#### Stage 7：终局抉择

- 收齐 `Main Archive` 所需日志并完成西线/南线调查闭环
- 回到飞船与洛希交互，正式进入结局分支点
- 在英雄、和平、定居三条路线中做出主动选择
- 选择后再执行对应的最终路线内容

### 结局体系

#### 英雄救援

- 在飞船与 `Loxi` 选定路线后，玩家沿正常世界路线推进北线
- 在西北遗迹内完成守卫战
- 守卫战结束后，英雄路线目标会重新指向 `Signal Tower`
- 由玩家手动完成最后发射

#### 和平救援

- 在飞船与 `Loxi` 确认路线
- 携带 `信号放大器（Signal Amplifier）` 稳定塔楼

#### 异星定居

- 在飞船与 `Loxi` 明确确认放弃求救
- 长期生存

#### 失败结局/死亡结束

- 生命值耗尽后本轮立即结束
- 不再保留"三次死亡容错"
- 玩家只能选择重新开始或读档

### 日志与归档系统

日志不再提供血量、攻击、补氧或其他直接属性奖励。日志的价值落在 `剧情理解`、`任务归档`、`终局前判断` 与 `结算价值` 上。

#### 日志构成

- `3` 份飞船内起始日志：用于建立坠毁后的初始认知
- `1` 份东线主线异常记录：用于坠毁线索/黑匣残留
- `5` 份西线日志：West Frontier、Survey Break、Canopy Hollow、Echo Basin、Last Camp 各 1 份
- `5` 份南线日志：South Collapse、Vent Galleries、Service Shafts、Purifier Ring、Root Vault 各 1 份

#### 归档分类

- **主线档案（Main Archive）**：负责终局主链与核心真相完成度（终局分支只检查此项）
- **补充档案（Supplemental Archive）**：负责人物、设施、立场和代价的补强

### 资源系统

#### 资源分层

- **基础建材**：木材、金属废料、矿石
- **生存辅助**：植物果实、壳果、发光苔、宁神蘑菇、特殊菌株、复苏口粮
- **区域功能资源**：异星藤蔓、壳果、废旧金属、防护纤维
- **终局资源**：能源核心、能量晶体、遗迹碎片、守卫鳞片

#### 刷新与保底规则

- 飞船内资源节点只作为教学与首轮保底资源，默认只可拾取一遍，不参与循环刷新
- 飞船外资源节点才参与刷新系统，并受刷新阈值与刷新次数上限约束
- Stage 1 - Stage 2 关键主线资源必须有可达保底

### 制作系统

制作不是收集图鉴，而是外出风险管理工具。关键配方对应的核心问题如下：

- **荧光棒**：降低夜间探索信息劣势
- **简易绳索**：缩短高耗氧路线，改善回撤效率
- **复苏口粮**：出门前在工作台密封的唯一快捷消耗品，同时负责回血、解毒与补氧
- **激光枪**：让中后期战斗从近身冒险转为可控处理
- **防护服**：提供过滤、防毒、减耗氧与危险区资格
- **信号放大器**：和平路线关键装置
- **野外营地**：野外续航与失败容错节点

### 怪物与战斗系统

战斗不是纯动作爽感，而是生存决策的一部分。玩家应当知道什么时候该战斗、什么时候该绕行或撤退。

#### 常规怪布置

- 常规怪数量固定，不再继续增加
- 常规怪只分布在西线与南线
- 东线 swamp 不再额外布置常规怪，风险来源以耗氧、毒雾、路线长度、补给压力为主
- 北线 ruins 不再布置常规怪，只保留 Monolith Ring 的守护者
- 最终 Boss 只存在于英雄路线触发后的北线西北遗迹守卫战区

### 结算与评分系统

游戏完成后进入结算页，采用 `1000 分制`，并给出 `A / B / C / D / E` 五档评级：

- A &gt;= 760
- B &gt;= 620
- C &gt;= 480
- D &gt;= 340
- E &lt; 340

评分构成：
- 主线推进：420
- 日志归档：220
- 生存表现：180
- 固定遭遇与 Boss 处理：120
- 完成任意成功结局：60

---

## 操作指南

| 按键 | 功能 |
|------|------|
| `WASD` / 方向键 | 移动 |
| `F` | 交互 / 修复 / 采集 / 工作台入口 |
| `Space` | 攻击 |
| `B` | 背包 |
| `N` | 洛希面板（当前任务 + 日志归档） |
| `M` | 地图 |
| `H` | 帮助 |
| `O` | 设置 |
| `X` | 快速使用复苏口粮 |
| `Esc` | 暂停 / 关闭当前界面 |

---

## 项目架构

### 混合架构

项目采用"玩法内核偏 C、外围编排偏 C++"的混合架构：

- **C 负责**：Player/TaskSystem/GameMap 等核心运行时状态、血量、氧气、状态结算、地图可走性、危险判定、移动阻挡判定、战斗、采集、阶段推进等热路径玩法逻辑
- **C++ 负责**：静态内容表与文本内容、状态图标、状态文案、Tooltip 数据表、存档 schema、兼容适配、平台层、UI 大块拆分与展示逻辑、高层会话、菜单、状态机编排

### 核心模块

#### 主循环与状态管理

- `src/main.c`：应用入口、窗口设置和主循环
- `src/game_manager.cpp`：顶层游戏状态机、帧编排、生命周期和顶层路由
- `src/game_session.cpp`：新游戏/开场完成/返回菜单生命周期流程、单生命死亡处理和重启状态重建
- `src/game_overlay.cpp`：Overlay 状态分发器

#### 游戏玩法

- `src/game_play.c`：游戏中状态下的活动游戏帧编排
- `src/game_play_input.c`：游戏中状态下的活动游戏输入循环
- `src/game_play_story.c`：围绕游戏更新捕获的故事触发快照
- `src/player.c`：玩家初始化、移动、动画时机和基础属性计算
- `src/player_status_runtime.c`：状态运行时存储、变更和衰减
- `src/player_consumables.c`：快速消耗品逻辑
- `src/task_system.c`：最小化的空间辅助/查询函数
- `src/task_progress.c`：目标和通讯器刷新
- `src/task_economy.c`：配方可见性和可制作性检查
- `src/task_update.c`：每帧任务运行时编排的薄层
- `src/task_survival.c`：昼夜循环和每日事件轮换
- `src/task_actions.cpp`：直接玩家攻击的战斗入口
- `src/task_interactions.cpp`：交互编排和首选目标评分
- `src/task_ship_interactions.cpp`：飞船控制台和房间交互逻辑
- `src/task_world_interactions.cpp`：荒野交互逻辑
- `src/task_crafting.cpp`：配方验证后的制作结果应用

#### 地图系统

- `src/map.c`：可见网格遍历和渲染编排
- `src/map_render_ground.c`：地面瓷砖渲染
- `src/map_render_props.c`：道具和地标渲染
- `src/map_layout.c`：共享地图布局辅助和 Map_Init 编排
- `src/map_layout_world.c`：世界地形和生物群系足迹放置
- `src/map_layout_ship.c`：飞船内部平面布置
- `src/map_runtime.c`：可走性、不透明度、瓷砖查找、危险查找和区域命名

#### UI 系统

- `src/ui_system.c`：共享 UI 绘制辅助
- `src/ui_layout.cpp`：所有 UI 矩形/布局计算器
- `src/ui_menu.cpp`：主菜单、暂停菜单、设置覆盖、死亡弹窗
- `src/ui_narrative_panels.cpp`：开场过场、故事场景面板
- `src/ui_ending_panel.cpp`：结局屏幕
- `src/ui_hud.cpp`：HUD 呈现
- `src/ui_info_panels.cpp`：通讯器、帮助覆盖
- `src/ui_backpack.cpp`：背包覆盖
- `src/ui_craft_panel.cpp`：制作覆盖
- `src/ui_log_reader.cpp`：日志归档覆盖
- `src/ui_map_panel.cpp`：完整地图覆盖
- `src/ui_save_slots_panel.cpp`：存档槽覆盖

#### 资源与持久化

- `src/assets.cpp`：顶层资源生命周期编排
- `src/assets_io.cpp`：可选纹理和字体加载辅助
- `src/assets_story_content.cpp`：开场过场、主线故事、日志故事和结局艺术加载
- `src/assets_gameplay_content.cpp`：游戏/世界纹理加载
- `src/audio_system.cpp`：可选提示和音乐加载
- `src/save_system.cpp`：持久化公共 API
- `src/save_storage.cpp`：持久化路径辅助
- `src/save_snapshot_loader.cpp`：快照加载流程
- `src/save_snapshot_codec.cpp`：游戏存档缓冲区的二进制编解码器
- `src/save_settings_codec.cpp`：设置缓冲区的二进制编解码器
- `src/save_legacy_adapter.cpp`：遗留存档布局的向后兼容解码
- `src/localization.cpp`：运行时语言状态、本地化 UI 查找

### 存档系统

- 支持 `16` 槽位手动存档
- 保存核心运行时状态
- 对旧版本存档保持兼容或可迁移
- 保存状态栏相关持续状态

---

## 当前状态

### 总体判断

项目已经具备稳定可玩的主循环，新一版 `Health + Oxygen + Status Bar` 体验已经进入实际运行代码。

当前可以明确认定已经成立的事情：

- `Stage 1 - Stage 7` 主线骨架可跑通
- 左下角 HUD 已显示 `状态栏 -&gt; 血量条 -&gt; 氧气条`
- 状态图标悬浮 Tooltip 已有运行时实现
- 中毒、漏氧、低氧、窒息、危急状态、过滤呼吸、营地恢复等状态已进入统一状态系统
- 游戏内天数按真实游玩时间累计，约 `5` 分钟算 `1` 天；睡觉会快进 `60` 秒，并只在跨天时提示新的一天开始
- 基地、营地、消耗品、防护服已经开始围绕新资源模型提供恢复和状态处理
- 主线推进已经进一步偏向 `求生 + 调查真相 + 回船确认路线`，而不是靠连续战斗强推
- 存档系统已能保存新资源与状态信息
- 旧版本存档仍有兼容映射逻辑
- 主菜单右上角现在会显示用户名/最高得分/存档数

### 已完成的收尾工作

✅ 已修正基地归档判定中的旧房间命名残留，回船总结与任务归档已稳定可信
✅ 已补真相拼装硬门槛（`Tasks_IsEndingPreCheckReady` + `endingArchiveReviewed`），终局选择需要先完成归档复核
✅ 已实现结局后闭环（4 选项菜单）和路线确认双重确认弹窗
✅ 已实现阶段化 BGM 播放改造（`Game_SelectMusicStage` + `Audio_SetMusicStage`）
✅ 已深化 `X1-X3` 的表现层：增强了三条交叉线索的完成提示叙事文本与展示时长
✅ 已微调东侧 swamp 的外围/深区节奏：Deep Basin 入口新增高风险警告提示
✅ 已补北侧终局区的事件反馈：BOSS 阶段切换（1→2→3）与击败均有战场提示消息与警告音效
✅ 已清理日志奖励相关的兼容字段与旧文案
✅ 死亡弹窗 bug 已修复（按键现在可以正常点击）
✅ 洛希终端和复苏口粮贴图接入代码已就绪

### 已拍板的设计事实

以下内容不再属于开放讨论，而是后续开发必须遵守的固定前提：

- 主资源固定为 `血量 + 氧气`
- 风险表达固定为 `状态栏`
- 左下角 HUD 固定为 `状态栏 -&gt; 血量条 -&gt; 氧气条`
- 飞船结构固定为 `中间主通道 + 上下两侧船舱`
- 世界布局主框架固定为 `北主线 / 东风险 / 西调查 / 南设施`
- 当前内容范围已完成西线 / 南线主链接入

---

## 开发指南

### 构建与测试

#### 使用 Makefile

```sh
make          # 构建
make clean    # 清理
make smoke    # 运行所有测试
```

#### 使用 CMake

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```

### 代码规范

- 公共头文件保持 C ABI 兼容
- 新增状态优先数据化，不要散落在多处条件分支
- HUD 与 Tooltip 文本尽量复用统一内容表
- 地图、飞船、剧情引导与平衡相关逻辑都必须按模块切片实现，不允许为了赶进度把新功能继续堆回单一大文件
- 如果文件开始拥有超过一个主要职责，在添加下一个功能之前先拆分

### 文档使用原则

- docs/design/ 和 docs/plans/ 中的活动文档优先于历史文档
- 旧扩展构想、旧阶段总结和旧重构计划只作为背景参考
- 若文档与当前代码冲突，以当前代码和活动文档为准

---

## 资源说明

### 资源概览

- **图片资源**：当前代码引用 146 张 PNG，目录中现有 151 张 PNG；当前图片运行时引用路径 146/146 全部完整存在，额外 5 张是主线正式命名兼容文件。
- **叙事图片**：当前叙事运行时槽位共 56 张，细分为 5 张开场分镜、30 张主线剧情图、14 张日志剧情图、7 张结局背景图；另保留 22 张历史主线兼容文件。
- **玩法/地图/角色/UI/敌人图片**：当前已接线并存在 71 张，已经按 characters/world/ui/enemies 四类路径收口。
- **音频资源**：当前代码主动加载 33 个运行时音频文件：19 个 wav 音效 + 14 个 ogg 音乐文件；当前 33/33 全部存在。资源目录另有 10 个阶段/路线 BGM 已入库但尚未接线。

### 资源加载策略

资源是可选的，并且必须始终有备用方案：
- 有文件时优先加载贴图/音频
- 缺文件时回退到程序化绘制或文字化剧情卡，或静音
- 单张缺图/音频不会阻止游戏运行

### 图片资源

本目录存放运行时可选加载的 PNG 资源，采用"缺图即回退"的安全策略。

#### 当前快照

审计日期：`2026-04-24`

- `resources/images/` 当前共落盘 151 张 PNG。
- 主线剧情图的正式命名口径统一为 `story_main_m##_&lt;scene_slug&gt;_v001.png`。
- `resources/images/story/main/` 当前共存在 52 个 PNG 文件：
  - 30 张已经具备正式命名文件。
  - 22 张仍作为历史 `main_##` 兼容文件保留。
- 主线剧情的逻辑槽位是 30 个；当前 30/30 都已经具备正式 PNG 文件。

#### 目录分类总览

**叙事资源 `resources/images/story/`（78 张）**：
- `story/cutscenes/`：5 张开场分镜
- `story/main/`：52 张文件，对应 30 个主线剧情槽位
- `story/logs/`：14 张日志剧情图
- `story/endings/`：7 张结局背景图

**角色资源 `resources/images/characters/`（3 张）**：
- `characters/player/`：
  - `my_astronaut.png`
  - `player_astronaut.png`
- `characters/npc/`：
  - `npc_loxi_terminal.png`

**世界资源 `resources/images/world/`（45 张）**：
- `world/tiles/terrain/`：12 张地表/地形图
- `world/tiles/props/`：14 张场景物件/交互设施图
- `world/tiles/barriers/`：3 张导流 barrier 图
- `world/nodes/`：16 张采集/掉落节点图

**UI 资源 `resources/images/ui/`（17 张）**：
- `ui/icons/`：9 张制作/装备/功能图标
- `ui/status/`：8 张状态栏图标

**敌人资源 `resources/images/enemies/`（8 张）**：
- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

#### 剧情对齐索引

**叙事图命名与剧情分段**

- `intro_01-intro_05`
  对应开场五张分镜，顺序与代码一一对应。
- `story_main_m01-story_main_m30`
  主线剧情唯一的正式命名口径，运行时槽位编号语义。
- `main_01-main_22`
  仅作为历史兼容文件保留。
- `log_01-log_03`
  对应飞船起始三份档案。
- `log_04-log_08`
  对应西线五份档案。
- `log_09`
  对应东线主线异常记录。
- `log_10-log_14`
  对应南线五份设施档案。
- `ending_01-ending_04`
  对应基础结局背景：定居/失败/强行救援/和平救援。
- `ending_05-ending_07`
  对应西南档案闭环后的增强结局背景。

#### 命名规则

- 主线剧情的逻辑编号统一以 `M01-M30` 为准，运行时代码按槽位映射资源，不再直接把单个历史文件名当成剧情身份。
- 主线剧情的正式文件名统一使用 `story_main_m##_&lt;scene_slug&gt;_v001.png`。
- 已落地的历史主线资源继续保留 `main_##_&lt;scene_slug&gt;.png`，但只承担兼容与回退职责。
- 日志与结局资源仍保持 `log_##_&lt;scene_slug&gt;.png`、`ending_##_&lt;scene_slug&gt;.png`。
- `scene_slug` 以剧情卡的稳定英文短语为准。

#### 完整文件清单

**`story/cutscenes/`**：
- `intro_01_unmarked_call.png`
- `intro_02_orbit_collapse.png`
- `intro_03_barely_alive.png`
- `intro_04_world_watches_back.png`
- `intro_05_stay_alive_first.png`

**`story/main/`**：
正式命名文件 30/30：
- `story_main_m01_air_for_one_more_day_v001.png`
- `story_main_m02_oxygen_cycle_restored_v001.png`
- `story_main_m03_loxi_full_sync_v001.png`
- `story_main_m04_airlock_opening_v001.png`
- `story_main_m05_signal_answers_back_v001.png`
- `story_main_m06_crash_not_accident_v001.png`
- `story_main_m07_deep_scan_online_v001.png`
- `story_main_m08_relic_pattern_decode_v001.png`
- `story_main_m09_east_wreck_confirmation_v001.png`
- `story_main_m10_basin_access_qualification_v001.png`
- `story_main_m11_west_route_confirmed_v001.png`
- `story_main_m12_survey_break_relay_v001.png`
- `story_main_m13_canopy_handoff_v001.png`
- `story_main_m14_echo_basin_reconstruction_v001.png`
- `story_main_m15_last_camp_positions_v001.png`
- `story_main_m16_south_facility_wakes_v001.png`
- `story_main_m17_vent_network_calibrated_v001.png`
- `story_main_m18_service_shaft_backbone_v001.png`
- `story_main_m19_purifier_ring_sequence_v001.png`
- `story_main_m20_root_vault_truth_v001.png`
- `story_main_m21_west_south_correlation_v001.png`
- `story_main_m22_loxi_conclusion_rewrite_v001.png`
- `story_main_m23_monolith_true_role_v001.png`
- `story_main_m24_north_route_commitment_v001.png`
- `story_main_m25_tower_not_button_v001.png`
- `story_main_m26_three_costs_revealed_v001.png`
- `story_main_m27_action_declaration_v001.png`
- `story_main_m28_heroic_route_commitment_v001.png`
- `story_main_m29_peaceful_route_commitment_v001.png`
- `story_main_m30_settlement_route_commitment_v001.png`

历史兼容文件 22：
- `main_01_air_for_one_more_day.png`
- `main_02_breathing_room_restored.png`
- `main_03_voice_in_the_wreck.png`
- `main_04_world_outside_opens.png`
- `main_05_signal_answers_back.png`
- `main_06_not_an_accident.png`
- `main_07_base_wakes_up.png`
- `main_08_pattern_is_alien.png`
- `main_09_ruins_notice_you.png`
- `main_10_sequence_holds.png`
- `main_11_last_barrier_breaks.png`
- `main_12_beacon_through_force.png`
- `main_13_beacon_through_understanding.png`
- `main_14_echo_basin_lock.png`
- `main_15_loxi_route_rewrite.png`
- `main_16_last_camp_archive.png`
- `main_17_purifier_ring_boot.png`
- `main_18_global_risk_drop.png`
- `main_19_root_vault_core.png`
- `main_20_trace_correlation.png`
- `main_21_loxi_sync_rewrite.png`
- `main_22_final_stance.png`

**`story/logs/`**：
- `log_01_impact_protocol.png`
- `log_02_split_roster.png`
- `log_03_pattern_not_wilderness.png`
- `log_04_west_signal_fragment_01.png`
- `log_05_survey_break_anchor_notes.png`
- `log_06_canopy_handoff_record.png`
- `log_07_echo_basin_topology_sketch.png`
- `log_08_last_camp_testament.png`
- `log_09_crash_recorder_black_box_residue.png`
- `log_10_purifier_outage_memo.png`
- `log_11_vent_calibration_handover.png`
- `log_12_service_shaft_sync_record.png`
- `log_13_purifier_ring_control_brief.png`
- `log_14_root_vault_core_dossier.png`

**`story/endings/`**：
- `ending_01_alien_settlement.png`
- `ending_02_failed_survival.png`
- `ending_03_heroic_rescue.png`
- `ending_04_peaceful_rescue.png`
- `ending_05_heroic_with_records.png`
- `ending_06_peaceful_with_repair.png`
- `ending_07_settlement_with_legacy.png`

**`characters/player/`**：
- `my_astronaut.png`
- `player_astronaut.png`

**`characters/npc/`**：
- `npc_loxi_terminal.png`

**`world/tiles/terrain/`**：
- `tile_base_floor.png`
- `tile_ship_corridor_floor.png`
- `tile_ship_room_floor.png`
- `tile_forest_ground.png`
- `tile_swamp_ground.png`
- `tile_swamp_outer_ground.png`
- `tile_swamp_deep_ground.png`
- `tile_echo_basin_floor.png`
- `tile_ruins_floor.png`
- `tile_ruins_path.png`
- `tile_ruins_ring_floor.png`
- `tile_tower_plateau_floor.png`

**`world/tiles/props/`**：
- `tile_tree_dense.png`
- `tile_rock_large.png`
- `tile_tech_table.png`
- `tile_storage_locker.png`
- `tile_bunk.png`
- `tile_workbench.png`
- `tile_oxygen_console.png`
- `tile_airlock_console.png`
- `tile_airlock_door.png`
- `tile_energy_console.png`
- `tile_comm_relay.png`
- `tile_signal_tower.png`
- `tile_crash_clue.png`
- `tile_monolith.png`

**`world/tiles/barriers/`**：
- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

**`world/nodes/`**：
- `node_wood.png`
- `node_ore.png`
- `node_metal_scrap.png`
- `node_fruit.png`
- `node_special_fungus.png`
- `node_energy_core.png`
- `node_glow_moss.png`
- `node_alien_vine.png`
- `node_shell_fruit.png`
- `node_junk_metal.png`
- `node_energy_crystal.png`
- `node_calm_mushroom.png`
- `node_protective_fiber.png`
- `node_relic_fragment.png`
- `node_boss_scale.png`
- `node_alien_slime.png`

**`ui/icons/`**：
- `icon_glow_stick.png`
- `icon_rope.png`
- `icon_reinforced_metal.png`
- `icon_laser_gun.png`
- `icon_protection_suit.png`
- `icon_signal_amplifier.png`
- `icon_field_camp.png`
- `icon_inventory_button.png`
- `icon_map_button.png`
- `icon_recovery_ration.png`（待添加）

**`ui/status/`**：
- `status_poisoned.png`
- `status_oxygen_leak.png`
- `status_low_oxygen.png`
- `status_suffocating.png`
- `status_critical_condition.png`
- `status_filtered.png`
- `status_oxygen_reserve.png`
- `status_camp_recovery.png`

**`enemies/`**：
- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

### 音频资源

本目录存放运行时可选加载的音频资源。

当前音频系统在 `src/audio_system.cpp` 中统一初始化，继续采用"缺失即静音"的安全策略：文件缺失或音频设备不可用时，程序仍可正常运行，只是对应音轨不会发声。

审计日期：`2026-04-26`

本次对代码的交叉检查结果如下：
- 当前代码共引用 33 个运行时音频文件：19 个 wav 音效 + 14 个 bgm_*.ogg
- 当前代码引用的音频文件在本次审计时已全部存在
- 当前资源目录额外包含 10 个已入库但尚未接线的新阶段/路线 BGM
- 音频目录已按 sfx/music 两级职责分层

运行时规则：
- `masterVolume` 会作用于整体输出，默认值为 1.0
- `sfxEnabled` 只会屏蔽 `Audio_PlayCue()` 触发的音效，不会关闭 `Audio_SetScene()` 驱动的 BGM 场景切换逻辑

#### 目录分层

**`sfx/`（19 个）**：
- `sfx/ui/`：8 个界面与交互音效
- `sfx/combat/`：4 个战斗/怪物提示音效
- `sfx/ambient/`：1 个环境提示音
- `sfx/footsteps/`：3 个脚步音效
- `sfx/endings/`：3 个成功结局音效

**`music/`（24 个，其中 14 个已接线，10 个未接线）**：
- `music/menu/`：2 个菜单 BGM
- `music/base/`：2 个基地 BGM
- `music/forest/`：2 个森林/西线路线 BGM
- `music/swamp/`：2 个沼泽/深沼 BGM
- `music/ruins/`：2 个遗迹/南线设施 BGM
- `music/boss/`：2 个 Boss/guardian arena BGM
- `music/endings/`：2 个结局场景 BGM
- `music/stages/`：8 个阶段 BGM，已入库未接线
- `music/routes/`：2 个路线 BGM，已入库未接线

#### 完整文件清单

**`sfx/ui/`**：
- `ui_confirm.wav`
- `ui_open.wav`
- `ui_close.wav`
- `ui_warning.wav`
- `ui_craft.wav`
- `ui_collect.wav`
- `ui_log.wav`
- `ui_repair.wav`

**`sfx/combat/`**：
- `combat_hurt.wav`
- `combat_melee.wav`
- `combat_laser.wav`
- `monster_roar.wav`

**`sfx/ambient/`**：
- `ambient_monolith.wav`

**`sfx/footsteps/`**：
- `step_forest.wav`
- `step_metal.wav`
- `step_swamp.wav`

**`sfx/endings/`**：
- `ending_heroic.wav`
- `ending_peaceful.wav`
- `ending_settlement.wav`

**`music/menu/`**：
- `bgm_menu_1.ogg`
- `bgm_menu_2.ogg`

**`music/base/`**：
- `bgm_base_1.ogg`
- `bgm_base_2.ogg`

**`music/forest/`**：
- `bgm_forest_1.ogg`
- `bgm_forest_2.ogg`

**`music/swamp/`**：
- `bgm_swamp_1.ogg`
- `bgm_swamp_2.ogg`

**`music/ruins/`**：
- `bgm_ruins_1.ogg`
- `bgm_ruins_2.ogg`

**`music/boss/`**：
- `bgm_boss_1.ogg`
- `bgm_boss_2.ogg`

**`music/endings/`**：
- `bgm_endings_1.ogg`
- `bgm_endings_2.ogg`

**`music/stages/`（已入库未接线）**：
- `bgm_stage3_into_the_wild_1.ogg`
- `bgm_stage3_into_the_wild_2.ogg`
- `bgm_stage4_rising_risk_1.ogg`
- `bgm_stage4_rising_risk_2.ogg`
- `bgm_stage5_power_mire_1.ogg`
- `bgm_stage5_power_mire_2.ogg`
- `bgm_stage7_final_choice_1.ogg`
- `bgm_stage7_final_choice_2.ogg`

**`music/routes/`（已入库未接线）**：
- `bgm_route_settlement_1.ogg`
- `bgm_route_settlement_2.ogg`

#### 区域/剧情对齐索引

**BGM 场景与地图口径**

当前运行时代码使用 `AudioScene` 按地图位置/场景语义切换 BGM。

- `music/menu/`：对应主菜单、开场前后界面与非游玩态
- `music/base/`：对应飞船基地/舱内整理阶段
- `music/forest/`：
  - 森林基础探索
  - 西线 West Frontier/Survey Break/Canopy Hollow/Echo Basin/Last Camp
- `music/swamp/`：
  - 东线外沼与常规沼泽推进
  - Deep Gate/Deep Basin
- `music/ruins/`：
  - 北线遗迹常规推进
  - 南线 South Collapse/Vent Galleries/Service Shafts/Purifier Ring/Root Vault
- `music/boss/`：
  - Stage 7 未击败守卫前的 Monolith Ring/Signal Tower Plateau 预战压迫段
  - 英雄路线隐藏 boss arena
- `music/endings/`：对应任意结局页背景音乐

**音效命名与用途**

- `sfx/ui/`：对应开关面板、确认、采集、日志、修复、制作等交互语义
- `sfx/combat/`：对应受击、近战、激光与怪物警戒
- `sfx/ambient/ambient_monolith.wav`：当前专用于石碑/遗迹环境反馈
- `sfx/footsteps/`：仅按地表材质分为 forest/metal/swamp
- `sfx/endings/`：分别对应 heroic/peaceful/settlement 三条成功路线的结局确认音

**已覆盖的行为**

- 菜单、面板、背包、地图、设置、暂停等界面的打开/关闭/确认
- 制作、采集、日志拾取、日志面板打开、修复类关键交互
- 森林/舱内金属地面/沼泽三类脚步声
- 近战、激光、受击、怪物警戒、石碑环境反馈
- 英雄、和平、定居三类成功结局音效
- 菜单、基地、普通森林、西线路线、外层沼泽、深层沼泽、北侧遗迹、南侧设施、Boss 预战区、Boss 战场、终局等 BGM 场景切换逻辑

### 待添加贴图

以下贴图代码已接入，放入对应路径即可生效：

- `resources/images/ui/icons/icon_recovery_ration.png` - 复苏口粮图标（建议尺寸：64×64 或 128×128）
- `resources/images/characters/npc/npc_loxi_terminal.png` - 洛希终端图标（建议尺寸：128×128 或 256×256）

### 维护规则

- 新图片先按职责选择目录，不再直接平铺到 `resources/images/` 根目录。
- 叙事图统一进入 `story/...`；角色图统一进入 `characters/...`；地图/节点统一进入 `world/...`；HUD/背包/状态图统一进入 `ui/...`；怪物图统一进入 `enemies/`。
- 新音效统一进入 `resources/audio/sfx/...`；新音乐统一进入 `resources/audio/music/...`。
- 功能型 UI 图标优先沿用 `icon_&lt;subject&gt;_button.png` 命名，避免与物品/状态类图标混淆。
- 只把文件放进目录不代表游戏一定会播，仍需同步确认代码是否真的接线。
- 文件名保持 ASCII，并严格区分大小写。
- `.DS_Store` 不计入资源统计。

### 资源目录结构

```
resources/
├── images/
│   ├── characters/
│   │   ├── npc/
│   │   │   └── npc_loxi_terminal.png
│   │   └── player/
│   │       ├── my_astronaut.png
│   │       └── player_astronaut.png
│   ├── enemies/
│   │   ├── mob_final_boss.png
│   │   ├── mob_fog_worm.png
│   │   ├── mob_raptor.png
│   │   ├── mob_relic_guard.png
│   │   ├── mob_sentinel_jelly.png
│   │   ├── mob_swamp_stalker.png
│   │   ├── mob_thorn_larva.png
│   │   └── mob_wing_bug.png
│   ├── story/
│   │   ├── cutscenes/
│   │   │   ├── intro_01_unmarked_call.png
│   │   │   ├── intro_02_orbit_collapse.png
│   │   │   ├── intro_03_barely_alive.png
│   │   │   ├── intro_04_world_watches_back.png
│   │   │   └── intro_05_stay_alive_first.png
│   │   ├── endings/
│   │   │   ├── ending_01_alien_settlement.png
│   │   │   ├── ending_02_failed_survival.png
│   │   │   ├── ending_03_heroic_rescue.png
│   │   │   ├── ending_04_peaceful_rescue.png
│   │   │   ├── ending_05_heroic_with_records.png
│   │   │   ├── ending_06_peaceful_with_repair.png
│   │   │   └── ending_07_settlement_with_legacy.png
│   │   ├── logs/
│   │   │   ├── log_01_impact_protocol.png
│   │   │   ├── log_02_split_roster.png
│   │   │   ├── log_03_pattern_not_wilderness.png
│   │   │   ├── log_04_west_signal_fragment_01.png
│   │   │   ├── log_05_survey_break_anchor_notes.png
│   │   │   ├── log_06_canopy_handoff_record.png
│   │   │   ├── log_07_echo_basin_topology_sketch.png
│   │   │   ├── log_08_last_camp_testament.png
│   │   │   ├── log_09_crash_recorder_black_box_residue.png
│   │   │   ├── log_10_purifier_outage_memo.png
│   │   │   ├── log_11_vent_calibration_handover.png
│   │   │   ├── log_12_service_shaft_sync_record.png
│   │   │   ├── log_13_purifier_ring_control_brief.png
│   │   │   └── log_14_root_vault_core_dossier.png
│   │   └── main/
│   ├── ui/
│   │   ├── icons/
│   │   │   ├── icon_field_camp.png
│   │   │   ├── icon_glow_stick.png
│   │   │   ├── icon_inventory_button.png
│   │   │   ├── icon_laser_gun.png
│   │   │   ├── icon_map_button.png
│   │   │   ├── icon_protection_suit.png
│   │   │   ├── icon_recovery_ration.png
│   │   │   ├── icon_reinforced_metal.png
│   │   │   ├── icon_rope.png
│   │   │   └── icon_signal_amplifier.png
│   │   └── status/
│   └── world/
│       ├── nodes/
│       └── tiles/
│           ├── barriers/
│           ├── props/
│           └── terrain/
└── audio/
    ├── music/
    │   ├── base/
    │   ├── boss/
    │   ├── endings/
    │   ├── forest/
    │   ├── menu/
    │   ├── routes/
    │   ├── ruins/
    │   ├── stages/
    │   └── swamp/
    └── sfx/
        ├── ambient/
        ├── combat/
        ├── endings/
        ├── footsteps/
        └── ui/
```

---

## 术语表

本文档用于统一当前项目的核心中英术语。

约束：
- 英文术语仍然是源码、配置和后续文本维护的默认源文本
- 中文术语必须保持一对一映射
- 玩家可见 UI、日志、任务文案都应尽量沿用本表

### 1. 角色与核心系统

| English | Simplified Chinese | 说明 |
| --- | --- | --- |
| Loxi | 洛希 | 飞船 AI 助手 |
| Portable Communicator | 便携终端 | 玩家打开任务与引导信息的界面 |
| Main Archive | 主线档案 | 推进终局选择所需的主线记录 |
| Supplemental Archive | 补充档案 | 补充人物、设施与背景细节的记录 |
| Archive Review | 归档复核 | `Stage 7` 在洛希终端先执行的档案复核动作 |
| Route Confirmation | 路线确认 | 归档复核后的正式终局路线选择 |
| Signal Tower | 信号塔 | 终局执行区域的关键设施 |
| guardian | 守卫 | 英雄路线需要面对的最终守卫 |
| monolith | 石碑 | 北线遗迹中的共鸣节点 |
| Monolith Ring | 石碑环区 | 石碑连成的终局准备区域 |

### 2. 主线阶段

| English | Simplified Chinese |
| --- | --- |
| Wake Up | 苏醒 |
| First Steps | 迈出第一步 |
| Into the Wild | 走向荒野 |
| Rising Risk | 风险升级 |
| Power Breakthrough | 动力突破 |
| Final Preparation | 最终准备 |
| Final Choice | 最终抉择 |

### 3. 路线与结局

| English | Simplified Chinese |
| --- | --- |
| Heroic Rescue | 强行救援 |
| Peaceful Rescue | 和平救援 |
| Alien Settlement | 异星定居 |
| Failed Survival | 生存失败 |

### 4. 区域与地点

#### 大区域

| English | Simplified Chinese |
| --- | --- |
| Ship Base | 飞船基地 |
| Crash Forest | 坠毁森林 |
| Spore Swamp | 孢子沼泽 |
| Ruins | 遗迹 |
| Echo Wilds | 回响荒野 |
| Subsurface Sink | 地下沉降带 |

#### 具体地点

| English | Simplified Chinese |
| --- | --- |
| Central Corridor | 中央走廊 |
| Cargo Hold | 货舱 |
| Crew Quarters | 船员舱 |
| Diagnostics | 诊断舱 |
| Terminal Bay | 终端舱 |
| Life Support | 生命维持舱 |
| Workshop | 工坊 |
| Power Bay | 动力舱 |
| Airlock Link | 气闸通道 |
| Ruins Approach | 遗迹前沿 |
| Signal Tower Plateau | 信号塔高台 |
| Outer Swamp Rim | 外沼边缘 |
| Flooded Detour | 积水绕路 |
| Deep Gate | 深层入口 |
| Deep Basin | 深潭区 |
| West Frontier | 西部前线 |
| Survey Break | 勘测断点 |
| Canopy Hollow | 林冠洼地 |
| Echo Basin | 回声盆地 |
| Last Camp | 最后营地 |
| South Collapse | 南部塌陷区 |
| Vent Galleries | 通风廊道 |
| Service Shafts | 维护井道 |
| Purifier Ring | 净化环区 |
| Root Vault | 根脉核心 |

### 5. 资源

| English | Simplified Chinese |
| --- | --- |
| Wood | 木材 |
| Ore | 矿石 |
| Metal Scrap | 金属残片 |
| Plant Fruit | 植物果实 |
| Special Fungus | 特殊菌株 |
| Glow Moss | 发光苔 |
| Alien Vine | 异星藤蔓 |
| Shell Fruit | 壳果 |
| Junk Metal | 废旧金属 |
| Energy Core | 能源核心 |
| Energy Crystal | 能量晶体 |
| Calming Mushroom | 宁神蘑菇 |
| Protective Fiber | 防护纤维 |
| Relic Fragment | 遗迹碎片 |
| Boss Scale | 守卫鳞片 |
| Alien Slime | 异星黏液 |
| Recovery Ration | 复苏口粮 |

### 6. 装备与制作

| English | Simplified Chinese |
| --- | --- |
| Glow Stick | 荧光棒 |
| Simple Rope | 简易绳索 |
| Weapon Calibration | 武器校准 |
| Laser Gun | 激光枪 |
| Protection Suit | 防护服 |
| Signal Amplifier | 信号放大器 |
| Field Camp | 野外营地 |

### 7. 状态

| English | Simplified Chinese |
| --- | --- |
| Poisoned | 中毒 |
| Oxygen Leak | 漏氧 |
| Low Oxygen | 低氧 |
| Suffocating | 窒息 |
| Critical Condition | 危急状态 |
| Filtered | 过滤呼吸 |
| Oxygen Reserve | 氧气储备 |
| Camp Recovery | 营地恢复 |

---

## 最终声明

项目范围已经冻结，后续工作不再扩展新地图、新剧情主线或大型新玩法系统。当前开发重心只有三类：

1. 平衡性调整
2. 稳定性与兼容性修复
3. 维护性整理与发布级收尾

