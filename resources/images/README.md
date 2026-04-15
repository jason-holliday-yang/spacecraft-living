# SpaceCraft Living 图片资源说明

本目录存放运行时可选加载的 PNG 资源。图片继续采用“缺图即回退”的安全策略：

- 有文件时优先加载贴图
- 缺文件时回退到程序化绘制或文字化剧情卡
- 单张缺图不会阻止游戏运行

## 当前快照

审计日期：`2026-04-14`

- 当前代码共引用 `111` 张 PNG 路径
- 当前目录中存在 `108` 张对应 PNG
- 当前剩余缺口为 `3` 张已接线但缺文件的 barrier 贴图
- 当前目录中没有“已落盘但完全未接线”的 PNG 文件

## 分类总览

### A. 已接线且存在

叙事图片 `48` 张：

- 开场分镜：`5`
- 主线剧情图：`22`
- 日志剧情图：`14`
- 结局背景图：`7`

玩法 / 地图 / 角色图片 `60` 张：

- 角色与 NPC：`3`
- 地表与场景物件：`26`
- 资源节点：`16`
- 制作 / 装备图标：`7`
- 怪物与 Boss：`8`

### B. 已接线但缺文件

当前仅剩以下 `3` 张：

- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

### C. 已入库未接线

- `0`

## 按内容职责分类

### 开场分镜 `cutscenes/`

当前已接线 `5` 张：

- `intro_01_unmarked_call.png`
- `intro_02_orbit_collapse.png`
- `intro_03_barely_alive.png`
- `intro_04_world_watches_back.png`
- `intro_05_stay_alive_first.png`

运行时入口：

- [`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp)
- [`src/ui_narrative_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp)

### 主线剧情图 `story/main/`

当前已接线 `22` 张：

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

当前触发层级：

- 基础主线节点
- `W4-W5`
- `S4-S5`
- `X1-X3`

运行时入口：

- [`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp)
- [`src/ui_narrative_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp)
- [`src/game_play_story.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play_story.c)

### 日志剧情图 `story/logs/`

当前已接线 `14` 张，已实现与游戏内 `14` 篇日志 `1:1` 对应：

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

运行时入口：

- [`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp)
- [`src/ui_narrative_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp)
- [`src/game_play_story.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play_story.c)

### 结局背景图 `story/endings/`

当前已接线 `7` 张：

- `ending_01_alien_settlement.png`
- `ending_02_failed_survival.png`
- `ending_03_heroic_rescue.png`
- `ending_04_peaceful_rescue.png`
- `ending_05_heroic_with_records.png`
- `ending_06_peaceful_with_repair.png`
- `ending_07_settlement_with_legacy.png`

当前结局图职责：

- 基础结局图：默认英雄 / 和平 / 定居 / 失败
- 强化结局图：在 `X3` 就绪后用于更完整的成功路线收束

运行时入口：

- [`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp)
- [`src/ui_ending_panel.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_ending_panel.cpp)

### 角色与 NPC

当前已接线 `3` 张：

- `my_astronaut.png`
- `player_astronaut.png`
- `npc_loxi_terminal.png`

说明：

- 玩家优先使用 `my_astronaut.png` 精灵表
- 缺失时回退到 `player_astronaut.png`

### 地表与场景物件

当前已接线且存在 `26` 张：

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

当前已接线但缺文件 `3` 张：

- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

运行时入口：

- [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp)
- [`src/map_render_ground_assets.c`](/Users/jason/Documents/SpaceCraftLivng/src/map_render_ground_assets.c)
- [`src/map_render_props.c`](/Users/jason/Documents/SpaceCraftLivng/src/map_render_props.c)

### 资源节点

当前已接线 `16` 张：

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

### 制作 / 装备图标

当前已接线 `7` 张：

- `icon_glow_stick.png`
- `icon_rope.png`
- `icon_reinforced_metal.png`
- `icon_laser_gun.png`
- `icon_protection_suit.png`
- `icon_signal_amplifier.png`
- `icon_field_camp.png`

### 怪物与 Boss

当前已接线 `8` 张：

- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

## 当前接线路径

- [`src/assets.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets.cpp)
  统一加载角色、叙事图、地图贴图与 UI 字体
- [`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp)
  统一加载开场 / 主线 / 日志 / 结局图
- [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp)
  统一加载地图、物件、节点、图标、怪物贴图
- [`src/game_play_story.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play_story.c)
  负责打开主线节点、日志、`W4-W5`、`S4-S5`、`X1-X3` 的剧情卡
- [`src/ui_narrative_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp)
  负责剧情卡文案与图片回退显示
- [`src/ui_ending_panel.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_ending_panel.cpp)
  负责结局背景图选择
- [`src/map_render_ground_assets.c`](/Users/jason/Documents/SpaceCraftLivng/src/map_render_ground_assets.c)
  负责飞船、沼泽、遗迹和 Echo Basin 地表贴图选择

## 维护规则

- 叙事图片命名继续使用稳定编号和 slug。
- 新增 PNG 如果已经被代码引用，必须同步更新本文档。
- 如果 PNG 已存在但还没接线，必须单独标成“已入库未接线”。
- 如果代码已引用但文件缺失，必须列入“已接线但缺文件”。
