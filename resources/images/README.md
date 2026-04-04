# SpaceCraft Living 图片资源命名清单

当前版本允许所有图片资源缺失，游戏会自动使用程序化回退绘制。

你后续只需要按下面的文件名把图片补到 `resources/images/`，代码就可以直接读取。

## 1. 角色与剧情

- `player_astronaut.png`
  用途：主角站立/通用角色图
- `npc_loxi_terminal.png`
  用途：洛希在基地核心或通讯器中的主视觉
- `ship_core_console.png`
  用途：飞船基地核心交互台
- `portable_comm.png`
  用途：便携通讯器图标
- `ship_log_icon.png`
  用途：飞船日志图标

## 2. 地图与场景瓦片

- `tile_base_floor.png`
- `tile_forest_ground.png`
- `tile_swamp_ground.png`
- `tile_deep_swamp.png`
- `tile_ruins_floor.png`
- `tile_tree_dense.png`
- `tile_rock_large.png`
- `tile_swamp_trap.png`
- `tile_poison_fog.png`
- `tile_workbench.png`
- `tile_signal_tower.png`
- `tile_relic_monolith.png`
- `tile_bridge_rope.png`
- `tile_field_camp.png`
- `tile_crash_debris.png`
- `tile_hidden_cache.png`

## 3. 资源采集点与掉落物

- `res_wood.png`
- `res_ore.png`
- `res_metal_scrap.png`
- `res_fruit.png`
- `res_special_fungus.png`
- `res_energy_core.png`
- `res_glow_moss.png`
- `res_alien_vine.png`
- `res_shell_fruit.png`
- `res_junk_metal.png`
- `res_energy_crystal.png`
- `res_calm_mushroom.png`
- `res_protective_fiber.png`
- `res_relic_fragment.png`
- `res_boss_scale.png`
- `res_alien_slime.png`

## 4. 装备、工具与道具图标

- `item_gathering_axe.png`
- `item_simple_knife.png`
- `item_glow_stick.png`
- `item_rope.png`
- `item_reinforced_metal.png`
- `item_laser_gun.png`
- `item_protection_suit.png`
- `item_signal_amplifier.png`
- `item_field_camp.png`
- `item_food_combo.png`
- `item_calm_combo.png`
- `item_weapon_coating.png`

## 5. 怪物

- `mob_thorn_larva.png`
- `mob_wing_bug.png`
- `mob_raptor.png`
- `mob_swamp_stalker.png`
- `mob_sentinel_jelly.png`
- `mob_fog_worm.png`
- `mob_relic_guard.png`
- `mob_final_boss.png`

## 6. UI 图标

- `ui_stamina.png`
- `ui_pressure.png`
- `ui_oxygen.png`
- `ui_day_cycle.png`
- `ui_event_positive.png`
- `ui_event_negative.png`
- `ui_stage_marker.png`
- `ui_crafting.png`
- `ui_help.png`
- `ui_map_marker.png`
- `ui_warning.png`

## 7. 推荐规格

- 瓦片资源：`32x32` 或 `64x64`
- 角色与怪物：`32x32`、`48x48` 或 `64x64`
- UI 图标：`32x32` 或 `64x64`
- 道具图标：`64x64`

## 8. 补图内容建议

建议整体视觉方向：

- 飞船基地：冷色金属、干净、安全感
- 坠毁森林：青绿偏冷、潮湿、发光植物
- 孢子沼泽：黄绿与暗蓝并存，强调泥泞、毒雾、危险感
- 遗迹区域：灰白石质、异星符号、低饱和高神秘感
- 洛希：温和、清晰、可信赖的科技辅助视觉

如果你后续要分批补图，建议优先顺序：

- 主角
- 场景瓦片
- 资源图标
- 怪物
- UI 图标
