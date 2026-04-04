# SpaceCraft Living 开发文档

> **文档版本**: v2.0  
> **最后更新**: 2026-04-04  
> **项目状态**: 核心功能完成，资源待补齐  
> **开发引擎**: C + raylib

---

## 📋 目录

1. [项目概述](#项目概述)
2. [技术架构](#技术架构)
3. [已完成功能](#已完成功能)
4. [未完成功能](#未完成功能)
5. [资源需求](#资源需求)
6. [开发路线图](#开发路线图)
7. [技术债务](#技术债务)
8. [构建与部署](#构建与部署)

---

## 项目概述

### 游戏简介

**SpaceCraft Living** 是一款 2D 俯视角生存解谜游戏。玩家扮演一名在异星坠毁的宇航员，通过探索、采集、制作和战斗，在氧气耗尽前修复飞船系统，最终选择三种不同路径触发结局。

### 核心特色

- **生存三指标**: 体力、压力、氧气相互制衡
- **7 阶段主线**: 从苏醒到终局的完整剧情链
- **昼夜循环**: 20 分钟完整周期，影响怪物行为
- **多结局系统**: 英雄救援、和平救援、异星定居
- **轻量化设计**: 无外部依赖，资源缺失可运行

### 目标平台

- ✅ macOS (当前开发平台)
- ⚠️ Linux (未测试)
- ⚠️ Windows (未测试)

---

## 技术架构

### 项目结构

```
SpaceCraftLiving/
├── src/                      # 源代码 (10 个 C 文件)
│   ├── main.c               # 程序入口
│   ├── game_manager.c       # 游戏状态管理
│   ├── map.c                # 地图系统
│   ├── player.c             # 玩家系统
│   ├── task_system.c        # 任务与 AI 系统
│   ├── ui_system.c          # UI 渲染
│   ├── save_system.c        # 存档系统
│   ├── assets.c             # 资源加载
│   ├── audio_system.c       # 音频系统
│   └── localization.c       # 本地化
├── include/                  # 头文件 (10 个.h 文件)
├── resources/                # 资源目录 (可选)
│   ├── images/              # 图片资源
│   └── audio/               # 音频资源
├── docs/                     # 文档目录
├── CMakeLists.txt           # CMake 配置
├── Makefile                 # Make 配置
└── README.md                # 项目说明
```

### 核心模块依赖

```
main.c
  └── game_manager.c
       ├── map.c
       │    └── assets.c
       ├── player.c
       │    └── map.c
       ├── task_system.c
       │    ├── map.c
       │    ├── player.c
       │    └── assets.c
       ├── ui_system.c
       │    ├── player.c
       │    ├── task_system.c
       │    └── assets.c
       ├── save_system.c
       └── audio_system.c
```

### 关键技术参数

| 参数 | 数值 | 说明 |
|------|------|------|
| 窗口尺寸 | 1440x900 | 默认分辨率 |
| 最小窗口 | 1180x760 | 最小允许尺寸 |
| 目标帧率 | 60 FPS | VSync 同步 |
| 地图尺寸 | 100x100 | 瓦片网格 |
| 瓦片大小 | 64x64 | 像素单位 |
| 相机缩放 | 1.20x | 固定倍率 |
| 昼夜周期 | 1200 秒 | 20 分钟完整循环 |

---

## 已完成功能

### ✅ 核心游戏系统 (完成度：85%)

#### 1.1 游戏状态管理
**状态**: 完全实现  
**文件**: `src/game_manager.c:12-16`

- 主菜单状态 (GAME_STATE_INTRO)
- 游戏进行中状态 (GAME_STATE_PLAYING)
- 结局状态 (GAME_STATE_ENDING)
- 状态切换逻辑完整

#### 1.2 主循环与窗口管理
**状态**: 完全实现  
**文件**: `src/main.c:40-59`

- 基于 raylib 的游戏循环
- 自适应窗口大小
- 60 FPS 目标帧率
- 多显示器适配
- 窗口自动居中

#### 1.3 输入处理系统
**状态**: 完全实现  
**文件**: `src/game_manager.c:284-410`

- 按键缓冲 (INPUT_BUFFER_TIME: 0.12f)
- 长按连发 (HOLD_REPEAT_INITIAL_DELAY: 0.26f)
- 方向键优先级处理
- WASD + 方向键双支持

#### 1.4 相机系统
**状态**: 完全实现  
**文件**: `src/game_manager.c:89-93`

- Camera2D 平滑跟随玩家
- 固定缩放比例 (1.20x)
- 屏幕中心偏移
- 自动适配窗口变化

#### 1.5 主线剧情系统
**状态**: 完全实现  
**文件**: `src/task_system.c:744-789`

**7 个主线阶段**:
1. **阶段 1 - 初始苏醒**: 采集木材和金属，修复氧气基础模块
2. **阶段 2 - 初次探索**: 制作荧光棒，完整修复氧气系统
3. **阶段 3 - 进入沼泽**: 修复通讯中继，解锁制作配方
4. **阶段 4 - 提升战力**: 制作激光枪和防护服，获取坠毁线索
5. **阶段 5 - 能源突破**: 获取能量核心，修复能源模块
6. **阶段 6 - 最终准备**: 收集遗物碎片，解锁信号增幅器
7. **阶段 7 - 终局抉择**: 选择救援或定居路径

#### 1.6 昼夜循环系统
**状态**: 完全实现  
**文件**: `src/task_system.c:325-358`

- **白天** (600 秒): 正常状态
- **黄昏** (120 秒): 压力缓慢累积
- **夜晚** (480 秒): 压力快速累积
- 完整周期 20 分钟
- 阶段自动切换

#### 1.7 随机事件系统
**状态**: 完全实现  
**文件**: `src/task_system.c:312-323`

**6 种轮换事件**:
- EVENT_HARVEST: 资源潮 (采集双倍)
- EVENT_CALM_BEASTS: 平静野兽 (怪物仇恨降低)
- EVENT_CLEAR_SKY: 晴朗天空 (无特殊效果)
- EVENT_SPORE_STORM: 孢子风暴 (未完全实现)
- EVENT_MONSTER_FRENZY: 怪物狂热 (仇恨和伤害提升)
- EVENT_DEVICE_FAULT: 设备故障 (氧气流失加速)

#### 1.8 结局系统
**状态**: 完全实现  
**文件**: `src/task_system.c:1618-1648`

**4 种结局**:
1. **英雄救援** (ENDING_HEROIC): 击败 Boss，手动激活信号塔
2. **和平救援** (ENDING_PEACEFUL): 制作增幅器，绕过 Boss
3. **异星定居** (ENDING_SETTLEMENT): 修复全部系统但不发送信号
4. **失败结局** (ENDING_FAILURE): 死亡 3 次或氧气耗尽

---

### ✅ 地图与场景系统 (完成度：70%)

#### 2.1 瓦片地图系统
**状态**: 完全实现  
**文件**: `src/map.c:94-323`

- 100x100 网格地图
- 20 种 Tile 类型定义
- 程序化绘制 fallback
- 可见区域裁剪优化
- 动态 Tile 渲染

#### 2.2 区域系统
**状态**: 完全实现  
**文件**: `src/map.c:376-393`

**5 大地图区域**:
- MAP_AREA_BASE: 飞船基地 (安全区)
- MAP_AREA_FOREST: 坠毁森林 (前期资源区)
- MAP_AREA_SWAMP_OUTER: 孢子沼泽外围
- MAP_AREA_SWAMP_DEEP: 孢子沼泽深处 (中毒区域)
- MAP_AREA_RUINS: 遗迹区域 (终局区)

#### 2.3 碰撞与寻路
**状态**: 完全实现  
**文件**: `src/map.c:329-351`

- Map_IsWalkable: 可行走判定
- Map_IsOpaque: 不透明判定
- Map_IsWithinBounds: 边界检测
- 基于 Tile 的阻挡逻辑

#### 2.4 区域解锁机制
**状态**: 完全实现  
**文件**: `src/map.c:462-474`

- 阶段 3: 气密门解锁 (Map_UnlockSwampOuter)
- 阶段 5: 沼泽深处解锁 (Map_UnlockSwampDeep)
- 阶段 6: 遗迹解锁 (Map_UnlockRuins)

#### 2.5 特殊地点系统
**状态**: 完全实现  
**文件**: `include/config.h:51-71`

**11 个关键交互点**:
- SHIP_CORE (37, 42): 飞船核心，提交修复
- WORKBENCH (44, 42): 工作台，高级制作
- AIRLOCK_CONSOLE (58, 42): 气密控制台
- COMM_RELAY (75, 35): 通讯中继
- CRASH_CLUE (71, 54): 坠毁线索
- ENERGY_CONSOLE (50, 56): 能源控制台
- SIGNAL_TOWER (90, 62): 信号塔，终局触发点
- MONOLITH_A/B/C: 3 个石碑，削弱 Boss

#### 2.6 营地系统
**状态**: 部分实现 ⚠️  
**文件**: `src/task_system.c:1246-1259`

**已实现**:
- 营地配方 (5 木材)
- 营地放置逻辑
- 休息恢复功能

**缺失**:
- 营地贴图未加载 (tile_field_camp.png)
- 营地视觉表现简单 (三角形 + 矩形)

---

### ✅ 生存系统 (完成度：85%)

#### 3.1 体力系统
**状态**: 完全实现  
**文件**: `src/player.c:175-181`

- 初始值 100，上限可成长 (BASE_MAX_STAMINA + bonus)
- 移动、采集、战斗消耗
- 低体力惩罚 (<30 时移动减速)
- 安全区自动恢复 (2.0/2 秒)
- 动态上限计算

#### 3.2 压力系统
**状态**: 完全实现  
**文件**: `src/player.c:183-192`

- 范围 0-100
- 多源累积：夜晚、遇敌、陷阱、毒雾
- **惩罚机制**:
  - 30+: 效率下降
  - 60+: 限制高级制作
  - 80+: 视觉干扰 (红色遮罩脉冲)
  - 100: 持续掉体力 (1.0/秒)

#### 3.3 氧气系统
**状态**: 完全实现  
**文件**: `src/task_system.c:360-380`

- 基础流失率：15/周期 (约 0.0125/秒)
- 修复等级影响:
  - 等级 0: 100% 流失
  - 等级 1: 70% 流失
  - 等级 2: 0% 流失 (完全修复)
- 低压惩罚：<30 时降低体力上限
- 归零触发失败结局

#### 3.4 中毒系统
**状态**: 完全实现  
**文件**: `src/player.c:202-211`

- 沼泽深处中毒风险 (HAZARD_POISON)
- 毒素累积速率：6.5/秒
- 防护服减伤 (60% 毒素)
- 蘑菇物品清除毒素
- MAX_POISON: 100

#### 3.5 临时增益系统
**状态**: 完全实现  
**文件**: `src/player.c:56-61`

**3 种 Buff**:
- 荧光棒夜视 (60 秒，未完全实现视觉)
- 速度提升 (30 秒，移动冷却×0.8)
- 压力免疫 (60 秒，压力不累积)

---

### ✅ 资源与制作系统 (完成度：80%)

#### 4.1 资源采集系统
**状态**: 完全实现  
**文件**: `src/player.c:213-236`

**16 种资源**:
- **基础资源**: 木材、矿石、金属碎片
- **辅助资源**: 果实、发光苔藓、藤蔓
- **功能资源**: 能量核心、水晶、保护纤维
- **特殊资源**: 遗物碎片、Boss 鳞片、外星粘液

**采集机制**:
- 体力消耗：8-10 点/次
- 事件加成：资源潮时双倍
- 节点再生：离开区域 120-300 秒后

#### 4.2 资源节点刷新
**状态**: 完全实现  
**文件**: `src/task_system.c:283-310`

- 80 个资源节点
- 区域差异化分布
- 离开区域后重生计时
- 特殊资源独立刷新 (300 秒)
- 普通资源刷新 (120 秒)

#### 4.3 制作系统
**状态**: 完全实现  
**文件**: `src/task_system.c:1136-1265`

**7 种配方**:
1. **荧光棒**: 1 木材 + 1 发光苔藓 (随时制作)
2. **绳索**: 1 木材 + 2 藤蔓 (阶段 3 解锁)
3. **强化金属**: 1 废金属 + 2 金属碎片 (工作台，阶段 4)
4. **激光枪**: 1 强化金属 + 2 矿石 (工作台，阶段 4)
5. **防护服**: 1 木材 + 2 藤蔓 + 1 保护纤维 (工作台，阶段 5)
6. **信号增幅器**: 1 能量核心 + 3 遗物碎片 (工作台，阶段 7)
7. **野外营地**: 5 木材 (随时制作)

**制作限制**:
- 工作台需求：高级道具必须在工作台制作
- 压力限制：压力≥60 时无法制作高级道具

#### 4.4 装备系统
**状态**: 部分实现 ⚠️  
**文件**: `src/player.c:66-76`

**6 种装备**:
- ✅ 荧光棒：已实现，夜视计时器
- ⚠️ 绳索：框架存在，跨越功能未实现
- ✅ 激光枪：已实现，远程攻击
- ✅ 防护服：已实现，减伤效果
- ✅ 信号增幅器：已实现，和平路线道具
- ✅ 营地：已实现，放置逻辑

**缺失**:
- 绳索跨越沼泽功能
- 装备视觉表现

#### 4.5 快速消耗系统
**状态**: 完全实现  
**文件**: `src/player.c:238-308`

- **Z 键**: 食物组合 (体力恢复 + 速度)
  - 果实 + 壳果：35 体力 + 30 秒速度
  - 单独使用：12-15 体力
- **X 键**: 镇静物品 (压力清除 + 免疫)
  - 特殊真菌 + 镇静蘑菇：50 压力清除 + 60 秒免疫
  - 单独使用：10-30 压力清除
- 重复使用递减效果

---

### ✅ 怪物与战斗系统 (完成度：75%)

#### 5.1 怪物 AI 系统
**状态**: 完全实现  
**文件**: `src/task_system.c:517-582`

**8 种怪物**:
| 怪物 | HP | 威胁度 | 特殊行为 |
|------|-----|--------|----------|
| 荆棘幼虫 | 28 | 低 | 无 |
| 飞翼虫 | 18 | 低 | 白天仇恨范围减半 |
| 猛禽 | 52 | 中 | 无 |
| 沼泽潜行者 | 48 | 中 | 夜晚仇恨范围×1.25 |
| 哨兵水母 | 72 | 高 | 无 |
| 雾虫 | 64 | 高 | 无 |
| 遗迹守卫 | 92 | 高 | Boss 护卫 |
| 最终 Boss | 220 | 极高 | 70% 血量召唤护卫 |

**AI 特性**:
- 仇恨范围：4-8 格 (根据类型和状态)
- 潜行规避：蹲下 + 森林区域可规避
- 追击逻辑：曼哈顿距离寻路
- 攻击冷却：0.9-1.2 秒

#### 5.2 战斗系统
**状态**: 完全实现  
**文件**: `src/task_system.c:834-881`

- **近战**: 匕首攻击 (16 基础伤害)
- **远程**: 激光枪 (28 基础伤害，5 格射程)
- **弱点机制**: Boss 背面攻击 2 倍伤害
- **石碑削弱**: 每个激活石碑增加 10% 伤害
- 攻击体力消耗：4 点/次

#### 5.3 怪物掉落系统
**状态**: 完全实现  
**文件**: `src/task_system.c:799-832`

**掉落表**:
- 荆棘幼虫 → 果实×1
- 飞翼虫 → 发光苔藓×1
- 猛禽 → 特殊真菌×1
- 沼泽潜行者 → 壳果×1 + 藤蔓×1
- 哨兵水母 → 能量核心×1
- 雾虫 → 镇静蘑菇×1 + 保护纤维×1
- 遗迹守卫 → 遗物碎片×1
- 最终 Boss → Boss 鳞片×3 + 能量核心×1

#### 5.4 Boss 战机制
**状态**: 部分实现 ⚠️  
**文件**: `src/task_system.c:537-540`

**已实现**:
- 70% 血量召唤 3 个遗迹守卫
- 背面弱点判定
- 石碑削弱机制

**缺失**:
- Boss 特殊攻击模式单一
- 阶段转换特效
- 技能前摇提示

---

### ✅ UI 与交互系统 (完成度：75%)

#### 6.1 HUD 系统
**状态**: 完全实现  
**文件**: `src/ui_system.c:150-201`

- 体力/压力/氧气三条形图
- 阶段信息面板
- 关键资源展示 (木材、矿石、核心等)
- 装备状态显示
- 消息提示 (256 字符，3 秒计时)
- 压力视觉干扰 (红色脉冲遮罩)

#### 6.2 主菜单
**状态**: 完全实现  
**文件**: `src/ui_system.c:203-225`

- 开始游戏
- 继续游戏 (无存档时灰色化)
- 退出
- 存档检测提示
- 背景动画 (粒子效果)

#### 6.3 暂停菜单
**状态**: 完全实现  
**文件**: `src/ui_system.c:227-242`

- 继续
- 保存
- 返回主菜单
- 半透明遮罩

#### 6.4 背包界面
**状态**: 完全实现  
**文件**: `src/ui_system.c:244-288`

- 16 种资源数量展示
- 6 种装备状态
- 快捷使用提示 (Z/X 键)
- 分类展示 (基础/辅助/功能/特殊)

#### 6.5 制作界面
**状态**: 完全实现  
**文件**: `src/ui_system.c:290-317`

- 7 种配方展示
- 数字键快速制作 (1-9)
- 材料需求说明
- 锁定配方隐藏
- 工作台依赖提示

#### 6.6 通讯器界面
**状态**: 完全实现  
**文件**: `src/ui_system.c:319-342`

- 在线/待机状态显示
- 当前阶段
- 昼夜信息
- 主线目标文本
- 按 N 键打开

#### 6.7 帮助界面
**状态**: 完全实现  
**文件**: `src/ui_system.c:344-374`

- 12 条操作提示
- 按键说明
- 完整控制列表

#### 6.8 结局界面
**状态**: 完全实现  
**文件**: `src/ui_system.c:376-389`

- 结局标题
- 结局描述文本
- 统计数据 (氧气/压力/死亡次数)
- 退出提示

#### 6.9 目标标记系统
**状态**: 部分实现 ⚠️  
**文件**: `src/game_manager.c:40-53`

**已实现**:
- 呼吸灯效果 (环形 + 外圈脉冲)
- 阶段目标坐标计算
- 动态标记位置

**缺失**:
- 小地图未实现
- 路径指引缺失
- 距离显示

---

### ✅ 存档与设置系统 (完成度：100%)

#### 7.1 存档系统
**状态**: 完全实现  
**文件**: `src/save_system.c:334-407`

**特性**:
- 二进制格式 (SAVE_MAGIC: "SCLSAV2")
- 原子写入防损坏 (临时文件 + rename)
- 数据校验 (MAGIC 检查 + 浮点数有效性)
- 完整状态快照:
  - 玩家位置、属性、资源
  - 任务进度、阶段、昼夜
  - 资源节点状态 (活性/重生次数)
  - 怪物状态 (位置/血量/阶段触发)
  - 日志收集状态

#### 7.2 读档系统
**状态**: 完全实现  
**文件**: `src/save_system.c:240-332`

- 存档检测
- 数据还原
- 地图状态恢复
- 错误处理 (校验失败返回 false)
- 世界重建

#### 7.3 设置存档
**状态**: 完全实现  
**文件**: `src/save_system.c:169-224`

- 音量保存 (0.0-1.0)
- 音效开关
- 独立文件 (SETTINGS_MAGIC: "SCLSET2")
- 默认值处理

#### 7.4 存档路径管理
**状态**: 完全实现  
**文件**: `src/save_system.c:149-167`

- **macOS**: `~/Library/Application Support/SpaceCraftLiving/`
- **降级路径**: `./SpaceCraftLivingData/` (HOME 环境变量缺失时)
- 目录自动创建
- 权限检查

---

### ✅ 音频系统 (完成度：35%)

#### 8.1 音频框架
**状态**: 完全实现  
**文件**: `src/audio_system.c:82-113`

- 音效加载 (8 种)
- BGM 加载 (8 首)
- 音量控制 (0.0-1.0)
- 场景音乐切换
- 可选资源加载 (文件缺失不报错)

#### 8.2 音效系统
**状态**: 框架完成，资源待补齐  
**文件**: `src/audio_system.c:94-101`

**已加载音效**:
- ui_confirm.wav - 确认
- ui_open.wav - 打开菜单
- ui_warning.wav - 警告
- ui_craft.wav - 制作
- ui_collect.wav - 采集
- ui_repair.wav - 修复
- combat_laser.wav - 激光枪
- ending_heroic.wav - 英雄结局

**缺失音效** (17 种):
- ui_close.wav
- ui_log.wav
- combat_melee.wav
- combat_hit.wav
- combat_player_hurt.wav
- combat_monster_die.wav
- env_poison.wav
- env_swamp.wav
- env_monolith.wav
- env_signal_tower.wav
- story_loxi_ping.wav
- ending_peaceful.wav
- ending_settlement.wav
- ending_failure.wav

#### 8.3 背景音乐系统
**状态**: 框架完成，资源待补齐  
**文件**: `src/audio_system.c:102-108`

**已加载 BGM**:
- bgm_menu.ogg - 主菜单
- bgm_base.ogg - 飞船基地
- bgm_forest.ogg - 坠毁森林
- bgm_swamp.ogg - 孢子沼泽
- bgm_ruins.ogg - 遗迹区域
- bgm_boss.ogg - Boss 战
- bgm_endings.ogg - 结局

**技术特性**:
- 音乐流式播放
- 场景平滑过渡
- 音量标准化 (0.72)

---

## 未完成功能

### 🔴 P0 - 核心体验 (高优先级)

#### 1. 绳索跨越功能
**优先级**: P0  
**预计工作量**: 2-3 天  
**技术难点**: 中等

**预期目标**:
- 允许玩家使用绳索跨越沼泽障碍
- 跨越后创建临时可通过路径
- 绳索消耗逻辑

**实现细节**:
```c
// 需要修改 map.c
bool Map_CrossSwampWithRope(GameMap *map, int targetX, int targetY);

// 需要修改 task_system.c
bool Tasks_UseRope(TaskSystem *tasks, Player *player, int targetX, int targetY);
```

**依赖**:
- 绳索道具已实现但未使用
- 沼泽障碍 Tile (TILE_BARRIER_SWAMP)

---

#### 2. 小型解谜系统
**优先级**: P0  
**预计工作量**: 3-4 天  
**技术难点**: 中等

**预期目标**:
- 3-5 个简单谜题，与石碑互动
- 线索系统
- 与主线整合

**实现细节**:
```c
typedef struct Puzzle {
    bool solved;
    int activationOrder[3]; // 石碑激活顺序
    int clueLocation;       // 线索位置
} Puzzle;

// 需要新增文件 src/puzzle.c
void Puzzle_Init(Puzzle *puzzle);
bool Puzzle_TryActivate(Puzzle *puzzle, int monolithIndex);
void Puzzle_DrawHint(const Puzzle *puzzle);
```

**依赖**:
- 3 个石碑已存在 (MONOLITH_A/B/C)
- 需要新增线索文本

---

#### 3. 日志收集系统 (完整)
**优先级**: P0  
**预计工作量**: 2 天  
**技术难点**: 低

**当前状态**: 框架已实现，奖励简单

**缺失内容**:
- 日志详细剧情文本 (5 个日志，每个 200-300 字)
- 收集进度追踪 UI
- 特殊奖励机制 (如解锁隐藏配方)

**实现细节**:
```c
// 需要修改 task_system.c
typedef struct ShipLog {
    bool active;
    bool collected;
    int gridX;
    int gridY;
    int rewardKind;
    char storyText[512]; // 新增剧情文本
} ShipLog;

// 需要修改 ui_system.c
void UI_DrawLogCollection(const ShipLog *logs, int count);
```

---

### 🟡 P1 - 体验增强 (中优先级)

#### 4. 怪物特殊攻击模式
**优先级**: P1  
**预计工作量**: 4-5 天  
**技术难点**: 高

**预期目标**:
- Boss 多种攻击模式转换
- 攻击前摇提示
- 躲避机制

**实现细节**:
```c
typedef enum BossAttack {
    BOSS_ATTACK_MELEE = 0,
    BOSS_ATTACK_CHARGE,    // 冲锋
    BOSS_ATTACK_SPAWN,     // 召唤
    BOSS_ATTACK_AOE        // 范围攻击
} BossAttack;

typedef struct Monster {
    // ...现有字段...
    BossAttack currentAttack; // 新增
    float attackTelegraph;    // 前摇计时器
} Monster;
```

---

#### 5. 陷阱系统 (完整)
**优先级**: P1  
**预计工作量**: 2-3 天  
**技术难点**: 低

**当前状态**: 伤害逻辑已实现

**缺失内容**:
- 陷阱视觉提示 (绊索、毒雾区)
- 触发特效
- 排雷机制

**实现细节**:
```c
// 需要修改 map.c
typedef enum HazardType {
    HAZARD_NONE = 0,
    HAZARD_TRIP,      // 绊索
    HAZARD_SWAMP,     // 沼泽
    HAZARD_POISON,    // 毒雾
    HAZARD_VISIBLE    // 新增：可见陷阱
} HazardType;

void Map_DrawHazardVisuals(const GameMap *map, float elapsedSeconds);
```

---

#### 6. 小地图系统
**优先级**: P1  
**预计工作量**: 3-4 天  
**技术难点**: 高

**预期目标**:
- 右上角小地图显示探索区域
- 战争迷雾
- 图标系统 (玩家、目标、资源)

**实现细节**:
```c
// 需要新增文件 src/minimap.c
typedef struct MiniMap {
    bool explored[MAP_HEIGHT][MAP_WIDTH]; // 探索状态
    int playerIconX, playerIconY;
    int objectiveIconX, objectiveIconY;
} MiniMap;

void MiniMap_Init(MiniMap *minimap);
void MiniMap_Update(MiniMap *minimap, const Player *player);
void MiniMap_Draw(const MiniMap *minimap, int screenWidth, int screenHeight);
```

---

#### 7. 更多音频资源
**优先级**: P1  
**预计工作量**: 外包 1-2 周  
**技术难点**: 低 (框架已实现)

**缺失列表** (17 种):
- **UI 音效** (2): ui_close.wav, ui_log.wav
- **战斗音效** (4): combat_melee.wav, combat_hit.wav, combat_player_hurt.wav, combat_monster_die.wav
- **环境音效** (4): env_poison.wav, env_swamp.wav, env_monolith.wav, env_signal_tower.wav
- **剧情音效** (1): story_loxi_ping.wav
- **结局音效** (3): ending_peaceful.wav, ending_settlement.wav, ending_failure.wav

**建议**: 外包给音效设计师，提供现有框架和命名规范

---

### 🟢 P2 - 锦上添花 (低优先级)

#### 8. 装备视觉表现
**优先级**: P2  
**预计工作量**: 3-4 天  
**技术难点**: 中等

**预期目标**:
- 穿防护服时角色颜色变化
- 拿激光枪时手臂/武器图层
- 荧光棒装备时光环效果

**实现细节**:
```c
// 需要修改 game_manager.c
void DrawPlayerWithEquipment(const Player *player, const AssetBundle *assets) {
    // 基础角色
    DrawPlayerBase(player, assets);
    
    // 防护服图层
    if (player->hasProtectionSuit) {
        DrawProtectionSuitOverlay(player);
    }
    
    // 激光枪图层
    if (player->hasLaserGun) {
        DrawLaserGun(player);
    }
}
```

---

#### 9. 天气系统
**优先级**: P2  
**预计工作量**: 4-5 天  
**技术难点**: 高

**预期目标**:
- 孢子暴、酸雨等天气效果
- 粒子系统
- 视野限制

**实现细节**:
```c
// 需要新增文件 src/weather.c
typedef enum WeatherType {
    WEATHER_CLEAR = 0,
    WEATHER_SPORE_STORM,
    WEATHER_ACID_RAIN
} WeatherType;

void Weather_Update(WeatherType *weather, float deltaTime);
void Weather_Draw(WeatherType weather, Camera2D camera);
```

---

#### 10. 成就系统
**优先级**: P2  
**预计工作量**: 2-3 天  
**技术难点**: 低

**预期目标**:
- 10-15 个成就追踪
- 解锁通知
- 成就列表 UI

**实现细节**:
```c
// 需要新增文件 src/achievement.c
typedef enum Achievement {
    ACH_FIRST_BLOOD = 0,
    ACH_REACH_FOREST,
    ACH_CRAFT_LASER_GUN,
    ACH_DEFEAT_BOSS,
    // ... 10-15 个成就
} Achievement;

void Achievement_CheckUnlock(Achievement *achievements, const Game *game);
void Achievement_DrawNotification(const Achievement *ach);
```

---

#### 11. 难度选择
**优先级**: P2  
**预计工作量**: 2 天  
**技术难点**: 低

**预期目标**:
- 简单/普通/困难三档
- 数值平衡调整
- 存档兼容性

**实现细节**:
```c
// 需要修改 config.h 和 game_manager.c
typedef enum GameDifficulty {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} GameDifficulty;

// 难度倍率
float GetStaminaCostMultiplier(GameDifficulty diff);
float GetMonsterDamageMultiplier(GameDifficulty diff);
```

---

## 资源需求

### 美术资源 (完成度：4%)

**需求清单** (46 项):

#### 角色与 NPC (2 项)
- [ ] player_astronaut.png - 主角
- [ ] npc_loxi_terminal.png - 洛希

#### 场景瓦片 (17 项)
- [ ] tile_base_floor.png
- [ ] tile_forest_ground.png
- [ ] tile_swamp_ground.png
- [ ] tile_deep_swamp.png
- [ ] tile_ruins_floor.png
- [ ] tile_tree_dense.png
- [ ] tile_rock_large.png
- [ ] tile_swamp_trap.png
- [ ] tile_poison_fog.png
- [ ] tile_workbench.png
- [ ] tile_signal_tower.png
- [ ] tile_relic_monolith.png
- [ ] tile_bridge_rope.png
- [ ] tile_field_camp.png
- [ ] tile_crash_debris.png
- [ ] tile_hidden_cache.png

#### 资源图标 (16 项)
- [ ] res_wood.png
- [ ] res_ore.png
- [ ] res_metal_scrap.png
- [ ] res_fruit.png
- [ ] res_special_fungus.png
- [ ] res_energy_core.png
- [ ] res_glow_moss.png
- [ ] res_alien_vine.png
- [ ] res_shell_fruit.png
- [ ] res_junk_metal.png
- [ ] res_energy_crystal.png
- [ ] res_calm_mushroom.png
- [ ] res_protective_fiber.png
- [ ] res_relic_fragment.png
- [ ] res_boss_scale.png
- [ ] res_alien_slime.png

#### 装备道具 (12 项)
- [ ] item_gathering_axe.png
- [ ] item_simple_knife.png
- [ ] item_glow_stick.png
- [ ] item_rope.png
- [ ] item_reinforced_metal.png
- [ ] item_laser_gun.png
- [ ] item_protection_suit.png
- [ ] item_signal_amplifier.png
- [ ] item_field_camp.png
- [ ] item_food_combo.png
- [ ] item_calm_combo.png
- [ ] item_weapon_coating.png

#### 怪物 (8 项)
- [x] mob_final_boss.png - **已实现**
- [ ] mob_thorn_larva.png
- [ ] mob_wing_bug.png
- [ ] mob_raptor.png
- [ ] mob_swamp_stalker.png
- [ ] mob_sentinel_jelly.png
- [ ] mob_fog_worm.png
- [ ] mob_relic_guard.png

#### UI 图标 (11 项)
- [ ] ui_stamina.png
- [ ] ui_pressure.png
- [ ] ui_oxygen.png
- [ ] ui_day_cycle.png
- [ ] ui_event_positive.png
- [ ] ui_event_negative.png
- [ ] ui_stage_marker.png
- [ ] ui_crafting.png
- [ ] ui_help.png
- [ ] ui_map_marker.png
- [ ] ui_warning.png

**推荐规格**:
- 瓦片资源：32x32 或 64x64
- 角色与怪物：32x32、48x48 或 64x64
- UI 图标：32x32 或 64x64
- 道具图标：64x64

---

### 音频资源 (完成度：32%)

**需求清单** (25 项):

#### 已实现 (8 项)
- [x] ui_confirm.wav
- [x] ui_open.wav
- [x] ui_warning.wav
- [x] ui_craft.wav
- [x] ui_collect.wav
- [x] ui_repair.wav
- [x] combat_laser.wav
- [x] ending_heroic.wav

#### BGM (7 项，框架已实现)
- [x] bgm_menu.ogg
- [x] bgm_base.ogg
- [x] bgm_forest.ogg
- [x] bgm_swamp.ogg
- [x] bgm_ruins.ogg
- [x] bgm_boss.ogg
- [x] bgm_endings.ogg

#### 缺失音效 (17 项)
- [ ] ui_close.wav
- [ ] ui_log.wav
- [ ] combat_melee.wav
- [ ] combat_hit.wav
- [ ] combat_player_hurt.wav
- [ ] combat_monster_die.wav
- [ ] env_poison.wav
- [ ] env_swamp.wav
- [ ] env_monolith.wav
- [ ] env_signal_tower.wav
- [ ] story_loxi_ping.wav
- [ ] ending_peaceful.wav
- [ ] ending_settlement.wav
- [ ] ending_failure.wav

**推荐规格**:
- 音效：WAV 格式，44.1kHz 或 48kHz
- BGM: OGG 格式，44.1kHz 或 48kHz
- 声道：单声道或立体声

---

## 开发路线图

### 第一阶段 (1-2 周): 核心补齐
**目标**: 完成 P0 功能，确保核心体验完整

- [ ] **绳索跨越功能** (2-3 天)
  - 实现绳索使用逻辑
  - 创建临时路径
  - 测试平衡性

- [ ] **小型解谜系统** (3-4 天)
  - 设计 3-5 个谜题
  - 实现石碑互动
  - 编写线索文本

- [ ] **日志收集完善** (2 天)
  - 编写 5 个日志剧情
  - 实现收集追踪 UI
  - 设计特殊奖励

- [ ] **陷阱可视化** (1-2 天)
  - 添加绊索/毒雾视觉效果
  - 实现触发特效

**交付物**: 可完整游玩的核心版本

---

### 第二阶段 (2-3 周): 体验优化
**目标**: 提升游戏品质和用户体验

- [ ] **Boss 技能丰富化** (4-5 天)
  - 设计 3-4 种攻击模式
  - 实现前摇提示
  - 调整难度曲线

- [ ] **小地图系统** (3-4 天)
  - 实现战争迷雾
  - 添加图标系统
  - 优化性能

- [ ] **基础音效补齐** (外包 1 周)
  - 制作 10 种关键音效
  - 集成测试

**交付物**: 体验优化的 beta 版本

---

### 第三阶段 (1-2 周): 美术资源
**目标**: 补齐关键美术资源

- [ ] **主角与怪物立绘** (3-4 天)
  - 主角宇航员设计
  - 8 种怪物立绘

- [ ] **关键 UI 图标** (2-3 天)
  - 三指标图标
  - 功能按钮图标

- [ ] **场景瓦片精修** (3-4 天)
  - 5 大区域地面瓦片
  - 树木/岩石装饰

**交付物**: 视觉完整的候选版本

---

### 第四阶段 (1 周): 打磨发布
**目标**: 最终打磨与发布准备

- [ ] **数值平衡调整** (2 天)
  - 测试难度曲线
  - 调整资源产出
  - 优化战斗数值

- [ ] **Bug 修复** (2 天)
  - 收集测试反馈
  - 修复关键问题

- [ ] **性能优化** (1 天)
  - 内存检测
  - 帧率优化

- [ ] **文档完善** (1 天)
  - 更新 README
  - 编写发布说明

**交付物**: 正式 release 版本

---

## 技术债务

### 已知问题

#### 1. 怪物寻路简单
**问题**: 当前使用曼哈顿距离，未实现 A*寻路  
**影响**: 怪物在复杂地形中可能卡住  
**优先级**: 中  
**解决方案**:
```c
// 引入 A*寻路算法
typedef struct PathNode {
    int gridX, gridY;
    float gCost, hCost, fCost;
    struct PathNode *parent;
} PathNode;

PathNode *AStar_FindPath(const GameMap *map, int startX, int startY, int endX, int endY);
```

---

#### 2. 资源重生逻辑
**问题**: 固定时间，未考虑玩家行为  
**影响**: 可能出现玩家守点刷资源  
**优先级**: 低  
**解决方案**:
```c
// 实现智能重生
typedef struct ResourceNode {
    // ...现有字段...
    float lastCollectedTime; // 上次采集时间
    int collectionCount;     // 采集次数统计
} ResourceNode;

// 根据采集频率动态调整重生时间
float CalculateRespawnTime(const ResourceNode *node);
```

---

#### 3. 存档兼容性
**问题**: 版本升级时缺少迁移机制  
**影响**: 更新后旧存档可能损坏  
**优先级**: 高  
**解决方案**:
```c
// 添加版本号
typedef struct SaveFileRecord {
    char magic[8];
    int32_t version; // 新增版本号
    // ...现有字段...
} SaveFileRecord;

// 实现迁移函数
bool SaveSystem_MigrateSave(SaveFileRecord *data, int32_t oldVersion);
```

---

#### 4. 内存管理
**问题**: 未检测内存泄漏  
**影响**: 长时间运行可能内存泄漏  
**优先级**: 中  
**解决方案**:
- 使用 Valgrind 检测: `valgrind --leak-check=full ./build/SpaceCraftLiving`
- 添加内存追踪宏
- 定期检测资源加载/卸载

---

### 优化建议

#### 1. 引入 A*寻路算法
**工作量**: 2-3 天  
**收益**: 怪物 AI 智能化

#### 2. 实现智能资源重生
**工作量**: 1-2 天  
**收益**: 游戏平衡性提升

#### 3. 添加存档版本号
**工作量**: 0.5 天  
**收益**: 避免存档损坏

#### 4. 使用 Valgrind 检测
**工作量**: 0.5 天  
**收益**: 确保内存安全

---

## 构建与部署

### 开发环境要求

- **操作系统**: macOS (推荐), Linux, Windows
- **编译器**: Clang 或 GCC
- **构建工具**: Make 或 CMake 3.18+
- **依赖库**: raylib

### 安装 raylib

**macOS**:
```bash
brew install raylib
```

**Linux**:
```bash
sudo apt-get install raylib-dev
```

**Windows**:
```bash
# 使用 vcpkg
vcpkg install raylib
```

### 构建方法

#### 方法 1: Make (推荐)

```bash
# 编译
make

# 运行
make run

# 清理
make clean

# 自定义 raylib 路径
make RAYLIB_PREFIX=/custom/path
```

#### 方法 2: CMake

```bash
# 配置
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build .

# 运行
./build/SpaceCraftLiving
```

#### 方法 3: Xcode

```bash
# 生成 Xcode 项目
cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=/opt/homebrew

# 打开项目
open build/xcode/SpaceCraftLiving.xcodeproj

# 在 Xcode 中选择 SpaceCraftLiving scheme
# Product -> Build
# Product -> Run
```

### 运行游戏

```bash
./build/SpaceCraftLiving
```

### 操作说明

| 按键 | 功能 |
|------|------|
| WASD / 方向键 | 移动 |
| E | 交互 (采集、修复、阅读) |
| Space | 攻击 |
| B | 打开背包 |
| Q | 打开制作 |
| N | 打开洛希通讯器 |
| H | 打开帮助 |
| Z | 快速使用食物 |
| X | 快速使用镇静物品 |
| C | 蹲下潜行 |
| Esc | 暂停/关闭菜单 |

### 打包发布

#### macOS
```bash
# 创建应用包
mkdir -p SpaceCraftLiving.app/Contents/MacOS
cp build/SpaceCraftLiving SpaceCraftLiving.app/Contents/MacOS/
cp -r resources SpaceCraftLiving.app/Contents/Resources/

# 创建 Info.plist
cat > SpaceCraftLiving.app/Contents/Info.plist << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>SpaceCraftLiving</string>
    <key>CFBundleIdentifier</key>
    <string>com.example.spacecraftliving</string>
    <key>CFBundleVersion</key>
    <string>2.0</string>
</dict>
</plist>
EOF
```

#### Linux
```bash
# 创建 tarball
tar -czf SpaceCraftLiving-linux.tar.gz build/SpaceCraftLiving resources/
```

#### Windows
```bash
# 使用 Inno Setup 创建安装程序
# 或使用 zip 打包
zip -r SpaceCraftLiving-win.zip build/SpaceCraftLiving.exe resources/
```

---

## 附录

### 版本历史

- **v2.0** (2026-04-04): 重构正式版，核心功能完成
- **v1.0** (未知): 初始版本，飞船舱室逃生流程

### 贡献者

- 开发：[待补充]
- 美术：[待补充]
- 音效：[待补充]

### 许可证

[待指定]

### 联系方式

[待补充]

---

*文档生成时间：2026-04-04*  
*下次更新：完成 P0 功能后*
