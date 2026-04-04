# SpaceCraft Living 音频资源命名清单

当前版本允许全部音频缺失，游戏会自动静音运行，不会报错。

后续把对应文件放进 `resources/audio/` 即可。

## 1. UI 与交互

- `ui_confirm.wav`
  用途：点击确认、选择按钮
- `ui_open.wav`
  用途：打开菜单、面板或通讯器
- `ui_close.wav`
  用途：关闭面板
- `ui_warning.wav`
  用途：高压、低氧、危险提示
- `ui_craft.wav`
  用途：成功制作
- `ui_collect.wav`
  用途：采集资源
- `ui_repair.wav`
  用途：修复飞船系统
- `ui_log.wav`
  用途：读取飞船日志

## 2. 战斗与环境

- `combat_melee.wav`
  用途：匕首近战攻击
- `combat_laser.wav`
  用途：激光枪发射
- `combat_hit.wav`
  用途：击中敌人
- `combat_player_hurt.wav`
  用途：玩家受伤
- `combat_monster_die.wav`
  用途：普通怪物死亡
- `env_poison.wav`
  用途：毒雾/中毒提示
- `env_swamp.wav`
  用途：沼泽下陷或泥泞踩踏
- `env_monolith.wav`
  用途：点亮石碑
- `env_signal_tower.wav`
  用途：激活信号塔

## 3. 剧情与结局

- `story_loxi_ping.wav`
  用途：洛希通讯弹出
- `ending_heroic.wav`
  用途：英雄救援结局
- `ending_peaceful.wav`
  用途：和平救援结局
- `ending_settlement.wav`
  用途：异星定居结局
- `ending_failure.wav`
  用途：遗憾结局

## 4. 背景音乐

- `bgm_menu.ogg`
  用途：主菜单
- `bgm_base.ogg`
  用途：飞船基地安全区
- `bgm_forest.ogg`
  用途：坠毁森林
- `bgm_swamp.ogg`
  用途：孢子沼泽
- `bgm_ruins.ogg`
  用途：遗迹区域
- `bgm_boss.ogg`
  用途：最终 Boss 战
- `bgm_endings.ogg`
  用途：结局段落

## 5. 推荐规格

- 音效：`wav`
- 背景音乐：`ogg`
- 采样率：`44.1kHz` 或 `48kHz`
- 声道：单声道或立体声都可

## 6. 补音优先顺序

- `ui_confirm.wav`
- `ui_collect.wav`
- `ui_repair.wav`
- `combat_laser.wav`
- `story_loxi_ping.wav`
- `bgm_base.ogg`
- `bgm_forest.ogg`
- `bgm_swamp.ogg`
- `bgm_ruins.ogg`
