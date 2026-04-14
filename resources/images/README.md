# SpaceCraft Living 图片资源说明

本目录存放运行时可选加载的 PNG 资源。`src/assets.cpp` 会统一尝试加载它们；单张图片缺失时，游戏会退回到程序化绘制或文本 / 渐变表现，不会因为缺图直接崩溃。

## 审计结论

审计日期：`2026-04-09`

本次对 `src/assets.cpp`、`src/assets_gameplay_content.cpp`、`src/assets_story_content.cpp`、`src/assets_io.cpp`、`src/resource_path.cpp`、`src/game_render.cpp`、`src/ui_menu.cpp`、`src/map_render_ground.c`、`src/map_render_props.c` 的交叉检查结果如下：

- 当前代码共引用 `87` 张 PNG 路径，其中目录里已存在 `77` 张
- 缺失的 `10` 张全部是 `2026-04-09` 新增接线的“地图可读性补强”可选贴图
- 这些缺失文件不会导致崩溃，运行时会继续退回当前程序化绘制
- 原有已落地的开场分镜、主线剧情图、日志剧情图、结局图、玩家、场景物件、资源节点、怪物、Boss、背包 / 制作图标仍然保持齐全
- 再次复核 `src/assets_gameplay_content.cpp`、`src/assets_io.cpp`、`src/resource_path.cpp` 后，没有发现地图资源路径解析或图片加载链路故障
- 当前地图资源的真实缺口更偏向 `区域可读性补强`，而不是 `文件存在但加载失败`
- 需要新增记录的一条资源规则是：`每一篇 log 都必须有独立剧情图`；当前代码只接了 `3` 张日志图，因此这部分现在属于“目标已确定、资源待补齐、接线待扩展”
- 新的剧情口径已经固定为“求生中搜集真相，并在回船后主动选择结局”，因此日志图应视为推进资源，而不是可有可无的奖励插画

额外需要注意的两条回退链路：

- 玩家优先加载 `my_astronaut.png`，按 `4 x 6` 精灵表解析；若缺失，再回退到 `player_astronaut.png` 静态贴图；再缺失才回退到程序化小人
- 开场分镜优先加载 `cutscenes/` 下的 `5` 张全屏图；若缺失，`src/ui_menu.cpp` 会回退到渐变背景 + 文案版分镜
- 主线剧情图 / 日志图 / 结局图优先加载 `story/` 下对应全屏图；若缺失，`src/ui_menu.cpp` 仍会回退到渐变背景 + 文案版剧情卡

当前新增但尚未补图的地图可读性贴图入口如下：

- `tile_ship_corridor_floor.png`
- `tile_ship_room_floor.png`
- `tile_swamp_outer_ground.png`
- `tile_swamp_deep_ground.png`
- `tile_ruins_path.png`
- `tile_ruins_ring_floor.png`
- `tile_tower_plateau_floor.png`
- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

## 当前已接入代码的图片

以下文件都会被当前代码主动尝试加载。

叙事图片命名现统一采用：

- `intro_<两位序号>_<场景 slug>.png`
- `main_<两位序号>_<场景 slug>.png`
- `log_<两位序号>_<日志标题 slug>.png`
- `ending_<两位序号>_<结局标题 slug>.png`

统一约束：

- slug 使用稳定英文短语，优先取剧情卡标题 / 日志标题，而不是临时实现备注
- 文件名保持 ASCII、小写、下划线分词
- 如果已接线资源需要改名，必须同一轮按 `先改文档 -> 再改代码 -> 最后改文件名` 执行，避免运行时断链

### 开场分镜（5）

- `cutscenes/intro_01_unmarked_call.png`
- `cutscenes/intro_02_orbit_collapse.png`
- `cutscenes/intro_03_barely_alive.png`
- `cutscenes/intro_04_world_watches_back.png`
- `cutscenes/intro_05_stay_alive_first.png`

### 主线剧情图（13）

- `story/main/main_01_air_for_one_more_day.png`
- `story/main/main_02_breathing_room_restored.png`
- `story/main/main_03_voice_in_the_wreck.png`
- `story/main/main_04_world_outside_opens.png`
- `story/main/main_05_signal_answers_back.png`
- `story/main/main_06_not_an_accident.png`
- `story/main/main_07_base_wakes_up.png`
- `story/main/main_08_pattern_is_alien.png`
- `story/main/main_09_ruins_notice_you.png`
- `story/main/main_10_sequence_holds.png`
- `story/main/main_11_last_barrier_breaks.png`
- `story/main/main_12_beacon_through_force.png`
- `story/main/main_13_beacon_through_understanding.png`

### 日志剧情图（3）

- `story/logs/log_01_impact_protocol.png`
- `story/logs/log_02_split_roster.png`
- `story/logs/log_03_pattern_not_wilderness.png`

说明：

- 以上 `3` 张仍是当前代码已经正式接线的日志剧情图。
- 但当前游戏内日志已经扩展到 `14` 篇，资源目标现已调整为：`每一篇 log 都要有一张独立剧情图，并承担该 log 的剧情推进表达`。
- 因此，下面的“日志剧情图全覆盖目标”属于新的资源规范；在真正扩展加载链路前，不能把它误记为“当前已接入代码”。

### 结局图（4）

- `story/endings/ending_01_alien_settlement.png`
- `story/endings/ending_02_failed_survival.png`
- `story/endings/ending_03_heroic_rescue.png`
- `story/endings/ending_04_peaceful_rescue.png`

### 玩家与角色（3）

- `my_astronaut.png`
- `player_astronaut.png`
- `npc_loxi_terminal.png`

### 地块与场景物件（18）

- `tile_base_floor.png`
- `tile_forest_ground.png`
- `tile_swamp_ground.png`
- `tile_ruins_floor.png`
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

### 资源节点（16）

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

### 装备与制作图标（7）

- `icon_glow_stick.png`
- `icon_rope.png`
- `icon_reinforced_metal.png`
- `icon_laser_gun.png`
- `icon_protection_suit.png`
- `icon_signal_amplifier.png`
- `icon_field_camp.png`

### 怪物与 Boss（8）

- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

## 当前渲染链路

- `src/assets.cpp`
  统一负责路径解析、可选贴图加载、透明边裁剪、超尺寸缩放，以及玩家精灵表帧分析
- `src/ui_menu.cpp`
  使用 `introSlides[]` 绘制开场分镜，使用 `storyMainScenes[] / storyLogScenes[]` 绘制剧情卡，使用 `storyEndingScenes[]` 绘制结局背景；缺图时统一退回文字化渐变演出
- `src/game_render.cpp`
  负责切换主菜单、开场分镜、剧情卡、结局页与正常游戏画面；玩家精灵表 / 静态贴图缺失时退回基础图形
- `src/map.c`
  使用地块、飞船内设施、控制台、信号塔、石碑等场景贴图
- `src/task_presentation.cpp`
  使用资源节点、普通怪物与最终 Boss 贴图
- `src/ui_inventory.cpp`
  使用资源节点图和装备图标绘制背包条目与制作条目

## 已接入代码的剧情演出图片

当前剧情图已经不再只是规划，而是已正式接入运行时流程：

- 新游戏开始后，会先进入 `cutscenes/` 下的 `5` 张开场分镜
- 进入游戏后，关键主线推进会触发 `story/main/` 下的剧情卡
- 当前已接线的 `3` 份日志在回收时会触发 `story/logs/` 下的日志剧情卡；后续目标是扩展到全部 `14` 篇日志
- 进入不同结局时，会使用 `story/endings/` 下的结局图作为终幕背景

新的叙事资源要求：

- 后续日志系统要承担推进玩法与传达真相的职责，因此 `每一篇 log` 都必须拥有自己的剧情图
- 日志剧情图不只是“拾取奖励插画”，而应当承担 `该篇 log 的关键情绪、信息重点、地点特征与推进语义`
- 大部分日志会在任务完成、设施恢复或调查节点闭环后才解锁，因此对应剧情图也要服务“完成这一步后发生了什么”
- 当日志图数量扩展后，资源文档要继续区分：`当前已接线日志图` 与 `目标应补齐日志图`

当前主线剧情图对应的运行时触发点如下：

- `main_01_air_for_one_more_day.png`
  第一次修复氧气控制台
- `main_02_breathing_room_restored.png`
  氧气系统完全恢复
- `main_03_voice_in_the_wreck.png`
  第一次同步 Loxi 终端 / 解锁通讯器
- `main_04_world_outside_opens.png`
  气闸开启，正式离开基地
- `main_05_signal_answers_back.png`
  通讯中继修复
- `main_06_not_an_accident.png`
  坠毁线索调查完成
- `main_07_base_wakes_up.png`
  能源修复完成
- `main_08_pattern_is_alien.png`
  交付遗迹碎片并完成 Loxi 分析
- `main_09_ruins_notice_you.png`
  首次激活石碑
- `main_10_sequence_holds.png`
  三石碑全部点亮
- `main_11_last_barrier_breaks.png`
  最终 Boss 倒下
- `main_12_beacon_through_force.png`
  英雄救援路线启动信号塔
- `main_13_beacon_through_understanding.png`
  和平救援路线启动信号塔

日志与结局图对应关系如下：

- `log_01_impact_protocol.png`
  第一份日志《Impact Protocol》
- `log_02_split_roster.png`
  第二份日志《Split Roster》
- `log_03_pattern_not_wilderness.png`
  第三份日志《Pattern, Not Wilderness》
- `ending_01_alien_settlement.png`
  定居结局
- `ending_02_failed_survival.png`
  失败结局
- `ending_03_heroic_rescue.png`
  英雄救援结局
- `ending_04_peaceful_rescue.png`
  和平救援结局

## 日志剧情图全覆盖目标（资源规范，未全部接线）

从本轮开始，日志图采用 `一篇 log 对应一张剧情图` 的资源规范。推荐命名如下：

- `story/logs/log_01_impact_protocol.png`
  对应《Ship Log 01: Impact Protocol》
- `story/logs/log_02_split_roster.png`
  对应《Ship Log 02: Split Roster》
- `story/logs/log_03_pattern_not_wilderness.png`
  对应《Ship Log 03: Pattern, Not Wilderness》
- `story/logs/log_04_west_signal_fragment_01.png`
  对应《Field Record: West Signal Fragment 01》
- `story/logs/log_05_survey_break_anchor_notes.png`
  对应《Field Record: Survey Break Anchor Notes》
- `story/logs/log_06_canopy_handoff_record.png`
  对应《Field Record: Canopy Handoff Record》
- `story/logs/log_07_echo_basin_topology_sketch.png`
  对应《Field Record: Echo Basin Topology Sketch》
- `story/logs/log_08_last_camp_testament.png`
  对应《Field Record: Last Camp Testament》
- `story/logs/log_09_crash_recorder_black_box_residue.png`
  对应《Crash Recorder: Black Box Residue》
- `story/logs/log_10_purifier_outage_memo.png`
  对应《Facility Record: Purifier Outage Memo》
- `story/logs/log_11_vent_calibration_handover.png`
  对应《Facility Record: Vent Calibration Handover》
- `story/logs/log_12_service_shaft_sync_record.png`
  对应《Facility Record: Service Shaft Sync Record》
- `story/logs/log_13_purifier_ring_control_brief.png`
  对应《Facility Record: Purifier Ring Control Brief》
- `story/logs/log_14_root_vault_core_dossier.png`
  对应《Facility Record: Root Vault Core Dossier》

这组文件的资源要求如下：

- 每张图都要直接服务对应 log 的剧情推进，而不是只做泛用概念图
- 图像内容要能一眼体现该篇 log 的地点、人物动作或设施状态
- 允许延续当前全屏剧情卡风格，但必须保证不同 log 之间有明确区分度
- 文件名保持 ASCII，并与 log 顺序稳定绑定，避免后续实现接线时出现错位

## 预留但尚未接线的剧情目录

当前仍保留 `story/optional/` 作为可选剧情插图目录，适合后续加入以下非强制演出：

- `optional_01_laser_gun.png`
- `optional_02_protection_suit.png`
- `optional_03_field_camp.png`
- `optional_04_rope_shortcut.png`

这些更适合做装备里程碑、搭营提示图或半屏剧情卡，当前尚未计入“已接入代码的图片”统计。

## 叙事资源设计原则

后续新增剧情图时，尽量保持以下统一约束：

- 风格统一为清晰像素风 / 科幻生存 / 冷色调演出图
- 主角、飞船、Loxi 终端、异星遗迹的外观要在多张图之间保持一致
- 图片中不直接嵌入大段文字，说明文字放到游戏 UI 层叠加
- 优先服务当前已存在的玩法节点，不把图片做成和代码状态脱节的“宣传概念图”
- 日志剧情图要优先服务“求生中搜集真相”的推进体验，玩家应能从图像上感受到该篇 log 为什么重要
- 每篇 log 的剧情图都应强化该篇记录的独特信息点，不能大量复用同一张泛化场景图

## 当前尺寸约束

这些约束直接来自 `src/assets.cpp` 与 `include/config.h`，比早期占位文档更接近真实运行效果：

- 单格地块 / 节点：按 `TILE_SIZE = 80` 的单格尺度适配
- `3 x 3` 舱内装置：`npc_loxi_terminal.png`、`tile_workbench.png`、`tile_oxygen_console.png`、`tile_airlock_console.png`、`tile_energy_console.png` 会被压到 `240 x 240`
- `2 x 2` 世界交互物：`tile_comm_relay.png`、`tile_signal_tower.png`、`tile_crash_clue.png`、`tile_monolith.png` 会被压到 `160 x 160`
- UI 图标：会被压到 `104 x 104`
- 普通怪物：会被压到 `112 x 112`
- 玩家精灵表：继续要求 `4 x 6` 结构；代码会分析每格非透明区域并自动归一化帧框
- 开场分镜、主线剧情图、日志图、结局图：都按不超过 `1920 x 1080` 的全屏图思路准备最稳妥

## 战斗系统图片资源清单

当前战斗逻辑已经完整接入，但玩家战斗外观仍主要依赖基础角色图、怪物图与程序化反馈。若要把战斗系统从“可玩”补到“视觉完成”，建议按下面清单准备资源。

### 当前战斗系统已具备的图片

- 玩家基础精灵表：`my_astronaut.png`
- 玩家静态回退图：`player_astronaut.png`
- 激光枪背包 / 制作图标：`icon_laser_gun.png`
- 防护服背包 / 制作图标：`icon_protection_suit.png`
- 普通怪物图：`mob_thorn_larva.png`、`mob_wing_bug.png`、`mob_raptor.png`、`mob_swamp_stalker.png`、`mob_sentinel_jelly.png`、`mob_fog_worm.png`、`mob_relic_guard.png`
- 最终 Boss 图：`mob_final_boss.png`

### 战斗系统后续必补资源

以下资源最值得优先补齐；即使先不做完整攻击动作，也能把“玩家持枪战斗”补到可以正式上线的程度。

- `my_astronaut_laser.png`
  持枪状态玩家精灵表；推荐作为第一优先级资源
- `my_astronaut_suit.png`
  穿防护服状态玩家精灵表；用于战斗区与高危区的装备外观表达
- `my_astronaut_laser_suit.png`
  持枪且穿防护服的组合状态玩家精灵表；如果继续沿用“整张人物图切状态”的方案，这张图是完整战斗视觉闭环所必需的
- `fx_laser_muzzle.png`
  激光枪枪口闪光
- `fx_laser_beam.png`
  激光束 / 射线主体
- `fx_laser_hit.png`
  激光命中特效
- `fx_melee_swing.png`
  近战挥击特效；用于无枪时的攻击反馈
- `fx_hurt_flash.png`
  玩家或怪物受击闪光 / 火花

### Boss 战推荐补齐资源

这些资源当前可以先用程序绘制替代，但如果要让最终战达到“可宣传截图”的完成度，建议补齐。

- `fx_boss_telegraph_melee.png`
  Boss 近战前摇警示
- `fx_boss_telegraph_charge.png`
  Boss 冲锋路径警示
- `fx_boss_telegraph_spawn.png`
  Boss 召唤守卫前摇警示
- `fx_boss_telegraph_aoe.png`
  Boss 范围攻击预警圈
- `fx_boss_charge_trail.png`
  Boss 冲锋残影 / 尘迹
- `fx_boss_aoe_ring.png`
  Boss AOE 命中环 / 冲击波
- `fx_boss_hit_core.png`
  Boss 弱点受击反馈；适合配合终局双倍伤害时使用

### 状态异常与生存压力推荐资源

战斗系统与 `oxygen leak / poison / critical condition` 强耦合，这些图不是严格必需，但非常适合后续增强可读性。

- `fx_oxygen_leak.png`
  漏氧粒子 / 面罩破损感
- `fx_poison_burst.png`
  中毒命中喷溅
- `fx_poison_aura.png`
  中毒持续状态环绕特效
- `fx_critical_pulse.png`
  危急状态脉冲高亮
- `fx_shield_filter.png`
  防护服过滤层 / 护面反光；用于表达过滤减伤

### 玩家战斗精灵表规格

若继续沿用当前玩家精灵表接线方式，所有玩家战斗状态图都建议遵守以下约束：

- 文件格式：透明背景 PNG
- 网格结构：`4 x 6`
- 行顺序：下、左、右、上
- 列顺序：第 `1` 列待机，第 `2-6` 列移动
- 推荐单格尺寸：`128 x 128`
- 推荐整张尺寸：`768 x 512`
- 角色脚底位置在所有帧里保持一致，避免移动时抖动
- 左右方向建议单独绘制，不要直接镜像，以免枪械朝向、手位、背包位置不自然
- 枪口、手部、头盔反光等关键细节不要贴近单元格边缘，给自动裁切留安全边距

### 特效资源规格

战斗特效图推荐使用独立小图，而不是塞进玩家角色表里：

- 文件格式：透明背景 PNG
- 推荐尺寸：`128 x 128`、`192 x 192` 或 `256 x 256`
- 适合做单张贴图的资源：枪口火花、命中火花、毒雾团、AOE 警示圈、Boss 冲锋拖尾
- 若特效需要多帧动画，建议额外做成规则精灵表，并在文件名中明确列数，例如 `fx_laser_hit_4x4.png`

### 长期更优的资源组织方式

当前代码最容易接的是“整张人物图切状态”，也就是补：

- `my_astronaut_laser.png`
- `my_astronaut_suit.png`
- `my_astronaut_laser_suit.png`

但如果后续装备种类继续增加，更推荐升级到“分层资源”方案，避免组合状态数量爆炸。推荐预留：

- `player_body_base.png`
- `player_overlay_laser.png`
- `player_overlay_suit.png`

这三张也建议保持同样的 `4 x 6` 结构。这样后面即使再增加新装备，也不需要为每种装备组合都重画整套角色动作。

### 推荐补图优先级

如果资源紧张，建议按下面顺序交付：

1. `my_astronaut_laser.png`
2. `fx_laser_muzzle.png`
3. `fx_laser_beam.png`
4. `fx_laser_hit.png`
5. `fx_melee_swing.png`
6. `my_astronaut_suit.png`
7. `my_astronaut_laser_suit.png`
8. 全套 Boss telegraph / AOE / charge 特效
9. 全套状态异常表现特效

## 仍适合后续补齐的方向

除上面 `10` 张“已接线但当前缺图”的地图可读性贴图外，以下视觉内容仍主要依赖程序绘制或文字：

- HUD 状态图标
- 日志 / 档案专属图标
- 危险、警告、任务提示类图标
- 更丰富的飞船内装饰件
- 区域边界、过渡景观与环境细节件
- 更完整的玩家战斗状态图与 Boss 战特效图

## 按新地图与剧情补完需要补充的图片资源

> 补完总清单见 [`../../docs/design/WEST_SOUTH_RESOURCE_GAP.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_RESOURCE_GAP.md)。

为避免把“已接线但缺图”和“纯规划未接线”混在一起，下面分两类记录：

- 第一类：`当前代码已接线，但目录里还缺图`
- 第二类：`后续建议补齐，但当前仍未接线`

### A. 当前已接线但缺图的地图可读性贴图（10）

这 `10` 张就是当前图片资源的真实缺口，已经有代码入口，但目录里还没有 PNG：

- `tile_ship_corridor_floor.png`
- `tile_ship_room_floor.png`
- `tile_swamp_outer_ground.png`
- `tile_swamp_deep_ground.png`
- `tile_ruins_path.png`
- `tile_ruins_ring_floor.png`
- `tile_tower_plateau_floor.png`
- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

用途：

- 拉开飞船主通道与舱室地面的视觉差异
- 拉开外沼泽、深沼泽、北线遗迹路径、终局高台的视觉层次
- 让 barrier 从“程序化封锁块”升级成更清楚的世界边界提示

### B. 飞船内部重构资源（未接线规划）

为了体现 `中间主通道 + 上下船舱` 的新结构，建议新增：

- `tile_ship_corridor_wall.png`
- `tile_ship_door_closed.png`
- `tile_ship_door_open.png`
- `tile_medbay_console.png`
- `tile_support_station.png`
- `tile_storage_rack.png`
- `tile_cabin_terminal.png`
- `tile_cabin_divider.png`
- `tile_ship_window.png`

用途：

- 区分主通道与舱室
- 区分医疗 / 支援 / 储藏 / 终端等不同功能舱
- 让飞船内部布局更容易一眼读懂

### C. 北侧石碑 / 遗迹主线区资源（未接线规划）

建议新增：

- `tile_monolith_active.png`
- `tile_monolith_solved.png`
- `tile_ruins_gate.png`
- `tile_ruins_obelisk.png`
- `tile_signal_tower_active.png`
- `tile_signal_tower_peaceful.png`
- `tile_signal_tower_heroic.png`
- `tile_ruins_hazard_vent.png`
- `tile_ruins_control_node.png`

用途：

- 明确区分终局主线节点状态
- 强化北侧区域是“终局方向”的视觉识别
- 减少石碑、塔、遗迹在视觉上的同质化

### D. 东侧 swamp 分层资源（未接线规划）

建议新增：

- `tile_swamp_spore_vent.png`
- `tile_swamp_rope_anchor.png`
- `tile_field_camp_upgraded.png`
- `node_medical_spore.png`
- `node_filter_reed.png`

用途：

- 拉开外沼泽与深沼泽的视觉层次
- 强化 Rope、Field Camp、过滤与解毒资源的地图可读性

### E. 西线 / 南线第二轮场景资源（未接线规划）

为支撑 `W4-W5` 与 `S4-S5`，建议优先补齐以下场景图：

- `tile_echo_basin_floor.png`
- `tile_echo_beacon_array.png`
- `tile_last_camp_tent.png`
- `tile_last_camp_archive_box.png`
- `tile_purifier_ring_core.png`
- `tile_purifier_console.png`
- `tile_root_vault_gate.png`
- `tile_root_vault_archive.png`
- `tile_subsurface_cooling_pipe.png`

用途：

- 让西线第二轮从“路线补段”升级为“叙事场景可识别”
- 让南线第二轮的设施接管和真相揭示有明确视觉锚点
- 对齐 `Echo Basin / Last Camp / Purifier Ring / Root Vault` 的地图与剧情命名

### F. HUD 与辅助图标（未接线规划）

虽然当前状态栏支持文字占位，但为了提升体验，后续建议补齐：

- `ui_status_poisoned.png`
- `ui_status_oxygen_leak.png`
- `ui_status_low_oxygen.png`
- `ui_status_suffocating.png`
- `ui_status_critical_condition.png`
- `ui_status_filtered.png`
- `ui_status_oxygen_reserve.png`
- `ui_status_camp_recovery.png`
- `ui_warning_route.png`
- `ui_warning_hazard.png`
- `ui_guidance_prepare.png`

用途：

- 提高左下角状态栏的识别效率
- 提高路线警示和准备提示的可读性

### G. 第二轮剧情图与结局反哺图（未接线规划）

结合当前剧情补完方向，建议新增：

- `main_14_echo_basin_lock.png`
- `main_15_loxi_route_rewrite.png`
- `main_16_last_camp_archive.png`
- `main_17_purifier_ring_boot.png`
- `main_18_global_risk_drop.png`
- `main_19_root_vault_core.png`
- `main_20_trace_correlation.png`
- `main_21_loxi_sync_rewrite.png`
- `main_22_final_stance.png`
- `ending_05_heroic_with_records.png`
- `ending_06_peaceful_with_repair.png`
- `ending_07_settlement_with_legacy.png`

用途：

- 补齐 `W4-W5`、`S4-S5` 的关键剧情节点演出
- 支撑 `X1-X3` 的跨区联动剧情表达
- 让三条成功结局都能体现西线 / 南线补完后的差异化叙事
- 与“每篇 log 一张剧情图”的规则配合，保证调查推进与终局反哺都能被视觉化承接

## 注意事项

- 文件名保持 ASCII，并严格区分大小写
- 仅把 PNG 放进目录不代表已经生效，必须确认 `src/assets.cpp` 及消费端代码真的接线
- 对于尚未接线的 optional 剧情图，文档里应单列为“规划中”，不要混入“当前已接入代码的图片”
- 每次新增、移除或改名运行时图片时，都应同步更新本文件
- 日志图相关变更必须同时更新：`当前已接线数量`、`目标日志图总数`、`一篇 log 对应一张图的命名表`
