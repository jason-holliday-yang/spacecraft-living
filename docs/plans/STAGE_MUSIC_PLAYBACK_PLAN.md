# SpaceCraft Living 阶段化音乐播放改造计划（2026-04-26）

## 1. 文档目的

本文档用于指导下一轮音频播放逻辑改造：

- 把当前“按地图位置 / AudioScene 切换 BGM”的逻辑改为“按主线阶段 / 关键剧情状态切换 BGM”
- 让每个阶段的一首 BGM 尽量完整播放，减少玩家跨区域移动时的突兀切歌
- 接入新入库的 `10` 首阶段 / 路线 BGM

如果本文档与资源文档冲突，以当前工作区资源路径和本文档为准。  
如果本文档与当前运行时代码冲突，说明代码尚未完成本计划。

## 2. 当前问题

当前 BGM 主控链路为：

- [`src/game_play.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play.c) 每帧调用 `Audio_SetScene(&game->audio, Game_SelectAudioScene(game))`
- [`src/game_audio_scene.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_audio_scene.cpp) 根据玩家坐标、`MapArea`、`locationName`、Boss / Ending 状态返回 `AudioScene`
- [`src/audio_system.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/audio_system.cpp) 根据 `requestedScene` 做淡入淡出和曲目变体轮换

这会导致：

- 玩家在同一主线阶段内跨地图子区域时，BGM 会被位置语义打断
- 一首曲子刚进入高潮，玩家走到下一个区域后就会切到另一首
- 当前 `3` 秒 crossfade 只能缓解音量突兀，不能解决叙事情绪被坐标打断的问题

下一轮目标不是继续拉长 fade，而是改变主控语义：

- 主 BGM 由阶段 / 剧情状态控制
- 地图位置只保留为少数强制状态或后续环境层依据

## 3. 新资源状态

### 3.1 已入库未接线

以下 `10` 首新 BGM 已经落盘，但当前代码尚未加载：

| 用途 | 文件 |
| --- | --- |
| Stage 3 走向荒野，版本 1 | `resources/audio/music/stages/bgm_stage3_into_the_wild_1.ogg` |
| Stage 3 走向荒野，版本 2 | `resources/audio/music/stages/bgm_stage3_into_the_wild_2.ogg` |
| Stage 4 风险升级，版本 1 | `resources/audio/music/stages/bgm_stage4_rising_risk_1.ogg` |
| Stage 4 风险升级，版本 2 | `resources/audio/music/stages/bgm_stage4_rising_risk_2.ogg` |
| Stage 5 动力突破，版本 1 | `resources/audio/music/stages/bgm_stage5_power_mire_1.ogg` |
| Stage 5 动力突破，版本 2 | `resources/audio/music/stages/bgm_stage5_power_mire_2.ogg` |
| Stage 7 最终抉择，版本 1 | `resources/audio/music/stages/bgm_stage7_final_choice_1.ogg` |
| Stage 7 最终抉择，版本 2 | `resources/audio/music/stages/bgm_stage7_final_choice_2.ogg` |
| 定居路线，版本 1 | `resources/audio/music/routes/bgm_route_settlement_1.ogg` |
| 定居路线，版本 2 | `resources/audio/music/routes/bgm_route_settlement_2.ogg` |

### 3.2 仍可复用的既有 BGM

| 用途 | 先复用 |
| --- | --- |
| Menu / 非游玩态 | `resources/audio/music/menu/bgm_menu_1.ogg`、`bgm_menu_2.ogg` |
| Stage 1 苏醒 | `resources/audio/music/base/bgm_base_1.ogg` |
| Stage 2 迈出第一步 | `resources/audio/music/base/bgm_base_2.ogg` |
| Stage 6 最终准备 / 遗迹碎片 | `resources/audio/music/ruins/bgm_ruins_1.ogg`、`bgm_ruins_2.ogg` |
| 英雄路线 / Boss | `resources/audio/music/boss/bgm_boss_1.ogg`、`bgm_boss_2.ogg` |
| 和平路线 / 结局 | `resources/audio/music/endings/bgm_endings_1.ogg`、`bgm_endings_2.ogg` |

## 4. 目标播放口径

### 4.1 主控优先级

BGM 决策按下面优先级从高到低执行：

1. 结局已经触发：使用结局音乐
2. Boss 战 / Boss arena：使用 Boss 音乐
3. 已确认终局路线：使用路线音乐
4. 普通游玩：使用当前主线阶段音乐
5. 菜单 / 无游戏状态：使用菜单音乐

地图位置不再直接抢占主 BGM。  
`Forest Route`、`Deep Basin`、`Vent Galleries`、`Root Vault` 等地点后续可以作为环境音层依据，但不应直接导致主音乐切换。

### 4.2 阶段到音乐映射

| 游戏状态 | 主 BGM |
| --- | --- |
| Menu | `menu` 既有两首 |
| Stage 1: Wake Up | `base/bgm_base_1.ogg` |
| Stage 2: First Steps | `base/bgm_base_2.ogg` |
| Stage 3: Into the Wild | `stages/bgm_stage3_into_the_wild_1.ogg`、`_2.ogg` |
| Stage 4: Rising Risk | `stages/bgm_stage4_rising_risk_1.ogg`、`_2.ogg` |
| Stage 5: Power Breakthrough | `stages/bgm_stage5_power_mire_1.ogg`、`_2.ogg` |
| Stage 6: Final Preparation | `ruins/bgm_ruins_1.ogg`、`bgm_ruins_2.ogg` |
| Stage 7: Final Choice，未确认路线 | `stages/bgm_stage7_final_choice_1.ogg`、`_2.ogg` |
| Hero route，Boss 未完成 | `boss/bgm_boss_1.ogg`、`bgm_boss_2.ogg` |
| Peaceful route | 暂用 `endings/bgm_endings_1.ogg`，后续可独立新增 |
| Settlement route | `routes/bgm_route_settlement_1.ogg`、`_2.ogg` |
| Ending panel | `endings/bgm_endings_1.ogg`、`bgm_endings_2.ogg` |

### 4.3 切换策略

阶段音乐应遵守：

- 同一阶段内跨地图区域，不切主 BGM
- 阶段变化时允许 crossfade，但不要因为每帧地点变化反复触发
- 同一阶段两首版本可以在当前曲接近末尾时轮换
- Boss / Ending 属于明确剧情转场，允许立即或短 crossfade 切换
- 回菜单、死亡结算、读档进入新局时可以强制重置音乐状态

当前 `audio_system.cpp` 已经有：

- `kSceneTransitionDuration`
- `activeScene / requestedScene / pendingScene`
- `activeSceneVariant / pendingSceneVariant`
- `IsMusicNearEnd()`
- 同一 scene 的两个 variant 轮换

下一轮可以复用这套机制，但主输入应从 `AudioScene` 改成“音乐阶段”。

## 5. 推荐代码设计

### 5.1 新增音乐阶段枚举

在 [`include/audio_system.h`](/Users/jason/Documents/SpaceCraftLivng/include/audio_system.h) 增加：

```c
typedef enum AudioMusicStage {
    AUDIO_MUSIC_NONE = 0,
    AUDIO_MUSIC_MENU,
    AUDIO_MUSIC_STAGE_1_WAKE,
    AUDIO_MUSIC_STAGE_2_FIRST_STEPS,
    AUDIO_MUSIC_STAGE_3_WILD,
    AUDIO_MUSIC_STAGE_4_RISK,
    AUDIO_MUSIC_STAGE_5_POWER,
    AUDIO_MUSIC_STAGE_6_RELICS,
    AUDIO_MUSIC_STAGE_7_CHOICE,
    AUDIO_MUSIC_ROUTE_HERO,
    AUDIO_MUSIC_ROUTE_PEACEFUL,
    AUDIO_MUSIC_ROUTE_SETTLEMENT,
    AUDIO_MUSIC_BOSS,
    AUDIO_MUSIC_ENDING
} AudioMusicStage;
```

新增公开接口：

```c
void Audio_SetMusicStage(AudioManager *audio, AudioMusicStage stage);
```

保留 `AudioScene` 和 `Audio_SetScene()`，但下一轮主 BGM 不再由 `Audio_SetScene()` 驱动。  
如果暂时不做环境音层，`Audio_SetScene()` 可以先保留兼容，不再由主循环每帧调用。

### 5.2 AudioManager 字段调整

推荐保持当前“每首曲目一个 OptionalMusic 字段”的低风险方式，先不引入动态资源表。

在 `AudioManager` 中新增：

```c
AudioMusicStage activeMusicStage;
AudioMusicStage requestedMusicStage;
AudioMusicStage pendingMusicStage;
int activeMusicVariant;
int pendingMusicVariant;
OptionalMusic stage3WildLoop;
OptionalMusic stage3WildLoopAlt;
OptionalMusic stage4RiskLoop;
OptionalMusic stage4RiskLoopAlt;
OptionalMusic stage5PowerLoop;
OptionalMusic stage5PowerLoopAlt;
OptionalMusic stage7ChoiceLoop;
OptionalMusic stage7ChoiceLoopAlt;
OptionalMusic settlementLoop;
OptionalMusic settlementLoopAlt;
```

如果希望少改字段，也可以复用现有 `activeSceneVariant` 等变量，但长期上 `scene` 和 `music stage` 分开更清楚。

### 5.3 加载新资源

在 [`src/audio_system.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/audio_system.cpp) 的 `Audio_Init()` 中新增加载：

```c
audio->stage3WildLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage3_into_the_wild_1.ogg");
audio->stage3WildLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage3_into_the_wild_2.ogg");
audio->stage4RiskLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage4_rising_risk_1.ogg");
audio->stage4RiskLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage4_rising_risk_2.ogg");
audio->stage5PowerLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage5_power_mire_1.ogg");
audio->stage5PowerLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage5_power_mire_2.ogg");
audio->stage7ChoiceLoop = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage7_final_choice_1.ogg");
audio->stage7ChoiceLoopAlt = LoadOptionalMusicAsset("resources/audio/music/stages/bgm_stage7_final_choice_2.ogg");
audio->settlementLoop = LoadOptionalMusicAsset("resources/audio/music/routes/bgm_route_settlement_1.ogg");
audio->settlementLoopAlt = LoadOptionalMusicAsset("resources/audio/music/routes/bgm_route_settlement_2.ogg");
```

同步更新：

- `StopAllMusic()`
- `Audio_Shutdown()`
- 获取当前音乐 pair 的 helper
- 变体默认值 helper
- 音量更新 helper

缺失资源仍沿用“缺失即静音”的安全策略，不应让游戏崩溃。

### 5.4 新增选择函数

在 [`src/game_audio_scene.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_audio_scene.cpp) 或新建 `src/game_music_stage.cpp` 中实现：

```c
AudioMusicStage Game_SelectMusicStage(const Game *game);
```

推荐选择逻辑：

```text
if game == NULL:
    AUDIO_MUSIC_NONE

if game->tasks.ending != ENDING_NONE:
    AUDIO_MUSIC_ENDING

if player is in MAP_AREA_BOSS_ARENA:
    AUDIO_MUSIC_BOSS

if final route is HERO and boss not defeated:
    AUDIO_MUSIC_ROUTE_HERO

if final route is PEACEFUL:
    AUDIO_MUSIC_ROUTE_PEACEFUL

if final route is SETTLEMENT:
    AUDIO_MUSIC_ROUTE_SETTLEMENT

switch game->tasks.stage:
    1 -> AUDIO_MUSIC_STAGE_1_WAKE
    2 -> AUDIO_MUSIC_STAGE_2_FIRST_STEPS
    3 -> AUDIO_MUSIC_STAGE_3_WILD
    4 -> AUDIO_MUSIC_STAGE_4_RISK
    5 -> AUDIO_MUSIC_STAGE_5_POWER
    6 -> AUDIO_MUSIC_STAGE_6_RELICS
    7 or higher -> AUDIO_MUSIC_STAGE_7_CHOICE
default:
    AUDIO_MUSIC_STAGE_1_WAKE
```

注意：

- `Stage 7` 的普通状态使用 `AUDIO_MUSIC_STAGE_7_CHOICE`
- 只有玩家明确选定路线后，才进入路线音乐
- Boss arena 和结局仍高于阶段音乐
- 不要再用 `locationName` 抢主 BGM

### 5.5 主循环接线

在 [`src/game_play.c`](/Users/jason/Documents/SpaceCraftLivng/src/game_play.c) 中把：

```c
Audio_SetScene(&game->audio, Game_SelectAudioScene(game));
```

替换为：

```c
Audio_SetMusicStage(&game->audio, Game_SelectMusicStage(game));
```

菜单、开场、读档、重新开始相关入口也要同步：

- [`src/game_manager.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_manager.cpp)
- [`src/game_session.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_session.cpp)
- [`src/game_save_runtime.cpp`](/Users/jason/Documents/SpaceCraftLivng/src/game_save_runtime.cpp)

原先调用 `Audio_SetScene(AUDIO_SCENE_MENU)` 的地方，改为：

```c
Audio_SetMusicStage(&game->audio, AUDIO_MUSIC_MENU);
```

原先进入 gameplay 时设置 `AUDIO_SCENE_BASE` 的地方，改为根据当前游戏状态选择：

```c
Audio_SetMusicStage(&game->audio, Game_SelectMusicStage(game));
```

## 6. 测试要求

### 6.1 资源路径验证

新增测试或 smoke 覆盖：

- `resources/audio/music/stages/bgm_stage3_into_the_wild_1.ogg`
- `resources/audio/music/stages/bgm_stage3_into_the_wild_2.ogg`
- `resources/audio/music/stages/bgm_stage4_rising_risk_1.ogg`
- `resources/audio/music/stages/bgm_stage4_rising_risk_2.ogg`
- `resources/audio/music/stages/bgm_stage5_power_mire_1.ogg`
- `resources/audio/music/stages/bgm_stage5_power_mire_2.ogg`
- `resources/audio/music/stages/bgm_stage7_final_choice_1.ogg`
- `resources/audio/music/stages/bgm_stage7_final_choice_2.ogg`
- `resources/audio/music/routes/bgm_route_settlement_1.ogg`
- `resources/audio/music/routes/bgm_route_settlement_2.ogg`

### 6.2 选择逻辑测试

在 [`tests/game_session_smoke.c`](/Users/jason/Documents/SpaceCraftLivng/tests/game_session_smoke.c) 或独立音频 smoke 中覆盖：

- `stage = 3` 时选择 `AUDIO_MUSIC_STAGE_3_WILD`
- `stage = 4` 时选择 `AUDIO_MUSIC_STAGE_4_RISK`
- `stage = 5` 时选择 `AUDIO_MUSIC_STAGE_5_POWER`
- `stage = 7` 且未选路线时选择 `AUDIO_MUSIC_STAGE_7_CHOICE`
- 选择 settlement 路线后选择 `AUDIO_MUSIC_ROUTE_SETTLEMENT`
- Boss arena 高于普通阶段
- Ending 高于所有阶段和路线

### 6.3 行为回归测试

手动验证：

- Stage 3 从基地移动到外部、森林、西线边界时，主 BGM 不因地点变化切换
- Stage 5 进入深沼和离开深沼时，主 BGM 不被地图区域反复打断
- Stage 7 回船复核档案时保持 `Final Choice` 氛围
- 选择 settlement 后切到 `bgm_route_settlement_*`
- Boss 战和结局仍能按剧情转场切音乐

## 7. 文档同步要求

完成代码改造后，同步更新：

- [`resources/audio/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/audio/README.md)
  - 把新 `10` 首从“已入库未接线”改为“已接线且完整”
  - 更新运行时音频总数
  - 更新 BGM 口径为“阶段音乐主控”
- [`resources/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/README.md)
  - 更新音频快照
  - 更新已接线 / 未接线分类
- [`docs/design/CURRENT_STATUS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/CURRENT_STATUS.md)
  - 完成后再记录“阶段化 BGM 已落地”

在代码未完成前，不要把新音乐写成已接线运行时资源。
