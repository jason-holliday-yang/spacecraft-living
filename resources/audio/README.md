# SpaceCraft Living 音频资源说明

本目录存放运行时可选加载的音频资源。当前音频系统在 `src/audio_system.cpp` 中统一初始化，继续采用“缺失即静音”的安全策略：文件缺失或音频设备不可用时，程序仍可正常运行，只是对应音轨不会发声。

## 审计结论

审计日期：`2026-04-17`

本次对 [`src/audio_system.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/audio_system.cpp)、[`src/game_session.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_session.cpp)、[`src/game_play.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play.c)、[`src/game_overlay.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_overlay.cpp) 的交叉检查结果如下：

- 当前代码共引用 `33` 个运行时音频文件：`19` 个 `wav` 音效 + `14` 个 `bgm_*.ogg`
- 当前代码引用的音频文件在本次审计时已全部存在
- 音频目录已按 `sfx / music` 两级职责分层，便于后续按功能查找和替换
- 当前音频缺口已经从“文件是否齐全”转为“是否需要继续细分区域身份与剧情演出音色”

另外两条运行时规则保持不变：

- `masterVolume` 会作用于整体输出，默认值为 `1.0`
- `sfxEnabled` 只会屏蔽 `Audio_PlayCue()` 触发的音效，不会关闭 `Audio_SetScene()` 驱动的 BGM 场景切换逻辑

## 当前目录分层

### `resources/audio/sfx/`（`19` 个）

- `resources/audio/sfx/ui/`：`8` 个界面与交互音效
- `resources/audio/sfx/combat/`：`4` 个战斗 / 怪物提示音效
- `resources/audio/sfx/ambient/`：`1` 个环境提示音
- `resources/audio/sfx/footsteps/`：`3` 个脚步音效
- `resources/audio/sfx/endings/`：`3` 个成功结局音效

### `resources/audio/music/`（`14` 个）

- `resources/audio/music/menu/`：`2` 个菜单 BGM
- `resources/audio/music/base/`：`2` 个基地 BGM
- `resources/audio/music/forest/`：`2` 个森林 / 西线路线 BGM
- `resources/audio/music/swamp/`：`2` 个沼泽 / 深沼 BGM
- `resources/audio/music/ruins/`：`2` 个遗迹 / 南线设施 BGM
- `resources/audio/music/boss/`：`2` 个 Boss / guardian arena BGM
- `resources/audio/music/endings/`：`2` 个结局场景 BGM

## 按路径展开

### `resources/audio/sfx/ui/`

- `ui_confirm.wav`
- `ui_open.wav`
- `ui_close.wav`
- `ui_warning.wav`
- `ui_craft.wav`
- `ui_collect.wav`
- `ui_log.wav`
- `ui_repair.wav`

### `resources/audio/sfx/combat/`

- `combat_hurt.wav`
- `combat_melee.wav`
- `combat_laser.wav`
- `monster_roar.wav`

### `resources/audio/sfx/ambient/`

- `ambient_monolith.wav`

### `resources/audio/sfx/footsteps/`

- `step_forest.wav`
- `step_metal.wav`
- `step_swamp.wav`

### `resources/audio/sfx/endings/`

- `ending_heroic.wav`
- `ending_peaceful.wav`
- `ending_settlement.wav`

### `resources/audio/music/menu/`

- `bgm_menu_1.ogg`
- `bgm_menu_2.ogg`

### `resources/audio/music/base/`

- `bgm_base_1.ogg`
- `bgm_base_2.ogg`

### `resources/audio/music/forest/`

- `bgm_forest_1.ogg`
- `bgm_forest_2.ogg`

### `resources/audio/music/swamp/`

- `bgm_swamp_1.ogg`
- `bgm_swamp_2.ogg`

### `resources/audio/music/ruins/`

- `bgm_ruins_1.ogg`
- `bgm_ruins_2.ogg`

### `resources/audio/music/boss/`

- `bgm_boss_1.ogg`
- `bgm_boss_2.ogg`

### `resources/audio/music/endings/`

- `bgm_endings_1.ogg`
- `bgm_endings_2.ogg`

## 区域 / 剧情对齐索引

### BGM 场景与地图口径

- `music/menu/`
  对应主菜单、开场前后界面与非游玩态。
- `music/base/`
  对应飞船基地 / 舱内整理阶段。
- `music/forest/`
  - `AUDIO_SCENE_FOREST`：森林基础探索
  - `AUDIO_SCENE_FOREST_ROUTE`：西线 `West Frontier / Survey Break / Canopy Hollow / Echo Basin / Last Camp`
- `music/swamp/`
  - `AUDIO_SCENE_SWAMP`：东线外沼与常规沼泽推进
  - `AUDIO_SCENE_SWAMP_DEEP`：`Deep Gate / Deep Basin`
- `music/ruins/`
  - `AUDIO_SCENE_RUINS`：北线遗迹常规推进
  - `AUDIO_SCENE_RUINS_FACILITY`：南线 `South Collapse / Vent Galleries / Service Shafts / Purifier Ring / Root Vault`
- `music/boss/`
  - `AUDIO_SCENE_BOSS`：`Stage 7` 未击败守卫前的 `Monolith Ring / Signal Tower Plateau` 预战压迫段
  - `AUDIO_SCENE_BOSS_ARENA`：英雄路线隐藏 boss arena
- `music/endings/`
  对应任意结局页背景音乐。

### 音效命名与用途

- `sfx/ui/`
  对应开关面板、确认、采集、日志、修复、制作等交互语义。
- `sfx/combat/`
  对应受击、近战、激光与怪物警戒。
- `sfx/ambient/ambient_monolith.wav`
  当前专用于石碑 / 遗迹环境反馈。
- `sfx/footsteps/`
  仅按地表材质分为 `forest / metal / swamp`，不再额外按区域复制命名。
- `sfx/endings/`
  分别对应 `heroic / peaceful / settlement` 三条成功路线的结局确认音。

## 当前接线路径

- [`src/audio_system.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/audio_system.cpp)
  负责可选加载、场景音乐切换、音效播发、音量设置与静音回退
- [`src/game_session.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_session.cpp)
  负责菜单 / 载入 / 开局过程中的场景音乐切换，以及部分确认 / 警告 / 打开提示音
- [`src/game_play.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play.c)
  负责按地图地点与地表类型切换音乐场景，并触发脚步、采集、日志、修复、战斗、石碑与结局音效
- [`src/game_overlay.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_overlay.cpp)
  负责背包、地图、设置、暂停、制作、定居确认等覆盖层的打开 / 关闭 / 确认 / 制作提示音

## 当前已覆盖的行为

- 菜单、面板、背包、地图、设置、暂停等界面的打开 / 关闭 / 确认
- 制作、采集、日志拾取、日志面板打开、修复类关键交互
- 森林 / 舱内金属地面 / 沼泽三类脚步声
- 近战、激光、受击、怪物警戒、石碑环境反馈
- 英雄、和平、定居三类成功结局音效
- 菜单、基地、普通森林、西线路线、外层沼泽、深层沼泽、北侧遗迹、南侧设施、Boss 预战区、Boss 战场、终局等 BGM 场景切换逻辑

## 后续维护建议

- 新音效统一进入 `resources/audio/sfx/...`
- 新音乐统一进入 `resources/audio/music/...`
- 只把文件放进目录不代表游戏一定会播，仍需同步确认 [`src/audio_system.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/audio_system.cpp) 和调用侧是否真的接线
- 文件名保持 ASCII，并严格区分大小写
