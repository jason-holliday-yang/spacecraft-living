# SpaceCraft Living 图片资源说明

本目录存放运行时可选加载的 PNG 资源。图片继续采用“缺图即回退”的安全策略：

- 有文件时优先加载贴图
- 缺文件时回退到程序化绘制或文字化剧情卡
- 单张缺图不会阻止游戏运行

## 当前快照

审计日期：`2026-04-24`

- `resources/images/` 当前共落盘 `151` 张 PNG。
- 主线剧情图的正式命名口径已经统一为 `story_main_m##_<scene_slug>_v001.png`。
- `resources/images/story/main/` 当前共存在 `52` 个 PNG 文件：
  - `30` 张已经具备正式命名文件。
  - `22` 张仍作为历史 `main_##` 兼容文件保留。
- 主线剧情的逻辑槽位仍是 `30` 个；当前 `30 / 30` 都已经具备正式 PNG 文件。

## 目录分类总览

### 叙事资源 `resources/images/story/`（`78` 张）

- `resources/images/story/cutscenes/`：`5` 张开场分镜
- `resources/images/story/main/`：`52` 张文件，对应 `30` 个主线剧情槽位
- `resources/images/story/logs/`：`14` 张日志剧情图
- `resources/images/story/endings/`：`7` 张结局背景图

运行时入口：

- [`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp)
- [`src/ui_narrative_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp)
- [`src/game_play_story.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play_story.c)
- [`src/ui_ending_panel.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_ending_panel.cpp)

### 角色资源 `resources/images/characters/`（`3` 张）

- `resources/images/characters/player/`
  - `my_astronaut.png`
  - `player_astronaut.png`
- `resources/images/characters/npc/`
  - `npc_loxi_terminal.png`

运行时入口：

- [`src/assets.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets.cpp)
- [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp)

### 世界资源 `resources/images/world/`（`45` 张）

- `resources/images/world/tiles/terrain/`：`12` 张地表 / 地形图
- `resources/images/world/tiles/props/`：`14` 张场景物件 / 交互设施图
- `resources/images/world/tiles/barriers/`：`3` 张导流 barrier 图
- `resources/images/world/nodes/`：`16` 张采集 / 掉落节点图

运行时入口：

- [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp)
- [`src/map_render_ground_assets.c`](/Users/jason/Documents/SpaceCraftLivng/src/map_render_ground_assets.c)
- [`src/map_render_props.c`](/Users/jason/Documents/SpaceCraftLivng/src/map_render_props.c)

### UI 资源 `resources/images/ui/`（`17` 张）

- `resources/images/ui/icons/`：`9` 张制作 / 装备 / 功能图标
- `resources/images/ui/status/`：`8` 张状态栏图标

运行时入口：

- [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp)
- [`src/ui_hud_status.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_hud_status.cpp)
- [`src/ui_inventory_data.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_inventory_data.cpp)
- [`src/ui_hud.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_hud.cpp)
- [`src/ui_info_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_info_panels.cpp)

### 敌人资源 `resources/images/enemies/`（`8` 张）

- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

运行时入口：

- [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp)

## 剧情对齐索引

### 叙事图命名与剧情分段

- `intro_01-intro_05`
  对应开场五张分镜，顺序与 [`src/ui_narrative_panels.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp) 的 `kIntroSlideDefs` 一一对应。
- `story_main_m01-story_main_m30`
  现在是主线剧情唯一的正式命名口径，也是运行时槽位编号语义，和 [`docs/design/MAINLINE_STORY_DETAIL_BRIEFS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/MAINLINE_STORY_DETAIL_BRIEFS.md) 保持一致。
- `main_01-main_22`
  仅作为历史兼容文件保留，不再视为主线剧情的正式命名来源。运行时会优先加载新的 `story_main_m##` 文件；若对应新文件尚未落盘，则才回退到兼容的旧 `main_##` 资源或程序化背景。
- `log_01-log_03`
  对应飞船起始三份档案。
- `log_04-log_08`
  对应西线五份档案。
- `log_09`
  对应东线主线异常记录《Crash Recorder: Black Box Residue》。
- `log_10-log_14`
  对应南线五份设施档案。
- `ending_01-ending_04`
  对应基础结局背景：定居 / 失败 / 强行救援 / 和平救援。
- `ending_05-ending_07`
  对应西南档案闭环后的增强结局背景。

### 当前命名规则

- 主线剧情的逻辑编号统一以 `M01-M30` 为准，运行时代码按槽位映射资源，不再直接把单个历史文件名当成剧情身份。
- 主线剧情的正式文件名统一使用 `story_main_m##_<scene_slug>_v001.png`。
- 已落地的历史主线资源继续保留 `main_##_<scene_slug>.png`，但只承担兼容与回退职责。
- 日志与结局资源仍保持 `log_##_<scene_slug>.png`、`ending_##_<scene_slug>.png`。
- `scene_slug` 以剧情卡的稳定英文短语为准，不重复再嵌入 `story`、`archive` 等泛词。

## 按路径展开

### `resources/images/story/cutscenes/`

- `intro_01_unmarked_call.png`
- `intro_02_orbit_collapse.png`
- `intro_03_barely_alive.png`
- `intro_04_world_watches_back.png`
- `intro_05_stay_alive_first.png`

### `resources/images/story/main/`

正式命名文件 `30 / 30`：

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

历史兼容文件 `22`：

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

### `resources/images/story/logs/`

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

### `resources/images/story/endings/`

- `ending_01_alien_settlement.png`
- `ending_02_failed_survival.png`
- `ending_03_heroic_rescue.png`
- `ending_04_peaceful_rescue.png`
- `ending_05_heroic_with_records.png`
- `ending_06_peaceful_with_repair.png`
- `ending_07_settlement_with_legacy.png`

### `resources/images/characters/player/`

- `my_astronaut.png`
- `player_astronaut.png`

### `resources/images/characters/npc/`

- `npc_loxi_terminal.png`

### `resources/images/world/tiles/terrain/`

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

### `resources/images/world/tiles/props/`

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

### `resources/images/world/tiles/barriers/`

- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

### `resources/images/world/nodes/`

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

### `resources/images/ui/icons/`

- `icon_glow_stick.png`
- `icon_rope.png`
- `icon_reinforced_metal.png`
- `icon_laser_gun.png`
- `icon_protection_suit.png`
- `icon_signal_amplifier.png`
- `icon_field_camp.png`
- `icon_inventory_button.png`
- `icon_map_button.png`

### `resources/images/ui/status/`

- `status_poisoned.png`
- `status_oxygen_leak.png`
- `status_low_oxygen.png`
- `status_suffocating.png`
- `status_critical_condition.png`
- `status_filtered.png`
- `status_oxygen_reserve.png`
- `status_camp_recovery.png`

### `resources/images/enemies/`

- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

## 维护规则

- 新图片先按职责选择目录，不再直接平铺到 `resources/images/` 根目录。
- 叙事图统一进入 `story/...`；角色图统一进入 `characters/...`；地图 / 节点统一进入 `world/...`；HUD / 背包 / 状态图统一进入 `ui/...`；怪物图统一进入 `enemies/`。
- 功能型 UI 图标优先沿用 `icon_<subject>_button.png` 命名，避免与物品 / 状态类图标混淆。
- 如果后续新增图片文件，请同步检查 [`src/assets.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets.cpp)、[`src/assets_story_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_story_content.cpp) 和 [`src/assets_gameplay_content.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/assets_gameplay_content.cpp) 的接线路径。
- `.DS_Store` 不计入资源统计。
