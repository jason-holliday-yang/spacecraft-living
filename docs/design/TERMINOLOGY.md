# SpaceCraft Living 中英术语表

本文档用于统一当前项目的核心中英术语。

约束如下：

- 英文术语仍然是源码、配置和后续文本维护的默认源文本
- 中文术语必须保持一对一映射，不再为同一英文名词混用多个译法
- 玩家可见 UI、日志、任务文案、设计文档都应尽量沿用本表
- 如需新增或调整译名，应同时检查 `/Users/jason/Documents/SpaceCraftLivng/src/localization.cpp`、`/Users/jason/Documents/SpaceCraftLivng/src/task_content.cpp`、`/Users/jason/Documents/SpaceCraftLivng/src/player_resources.c`、`/Users/jason/Documents/SpaceCraftLivng/src/recipe_catalog.cpp`、`/Users/jason/Documents/SpaceCraftLivng/src/player_status_text.c`

## 1. 角色与核心系统

| English | Simplified Chinese | 说明 |
| --- | --- | --- |
| Loxi | 洛希 | 飞船 AI 助手 |
| Portable Communicator | 便携终端 | 玩家打开任务与引导信息的界面 |
| Main Archive | 主线档案 | 推进终局选择所需的主线记录 |
| Supplemental Archive | 补充档案 | 补充人物、设施与背景细节的记录 |
| Signal Tower | 信号塔 | 终局执行区域的关键设施 |
| guardian | 守卫 | 英雄路线需要面对的最终守卫 |
| monolith | 石碑 | 北线遗迹中的共鸣节点 |
| Monolith Ring | 石碑环区 | 石碑连成的终局准备区域 |
| maintenance lattice | 维护格网 | 终局分析中反复出现的遗迹系统概念 |

## 2. 主线阶段

| English | Simplified Chinese |
| --- | --- |
| Wake Up | 苏醒 |
| First Steps | 迈出第一步 |
| Into the Wild | 走向荒野 |
| Rising Risk | 风险升级 |
| Power Breakthrough | 动力突破 |
| Final Preparation | 最终准备 |
| Final Choice | 最终抉择 |

## 3. 路线与结局

| English | Simplified Chinese |
| --- | --- |
| Heroic Rescue | 强行救援 |
| Peaceful Rescue | 和平救援 |
| Alien Settlement | 异星定居 |
| Failed Survival | 生存失败 |

## 4. 区域与地点

### 4.1 大区域

| English | Simplified Chinese |
| --- | --- |
| Ship Base | 飞船基地 |
| Crash Forest | 坠毁森林 |
| Spore Swamp | 孢子沼泽 |
| Ruins | 遗迹 |
| Echo Wilds | 回响荒野 |
| Subsurface Sink | 地下沉降带 |

### 4.2 具体地点

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
| Monolith Ring | 石碑环区 |
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

## 5. 资源

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

## 6. 装备与制作

| English | Simplified Chinese |
| --- | --- |
| Glow Stick | 荧光棒 |
| Simple Rope | 简易绳索 |
| Reinforced Metal | 强化金属 |
| Laser Gun | 激光枪 |
| Protection Suit | 防护服 |
| Signal Amplifier | 信号放大器 |
| Field Camp | 野外营地 |

## 7. 状态

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

## 8. 使用规则

- 文档里第一次出现核心术语时，推荐写成 `中文（English）` 或 `English（中文）`
- 运行时按钮、背包、状态名、地点名等短文本，以中文定名或英文本体直接显示，不再混写旧译名
- 如果某处必须保留英文开发代号，也应确保附近能看到对应中文名
