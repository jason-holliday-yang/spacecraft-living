# SpaceCraft Living 音频资源说明

本目录存放运行时可选加载的音频资源。当前音频系统在 `src/audio_system.cpp` 中统一初始化，继续采用“缺失即静音”的安全策略：文件缺失或音频设备不可用时，程序仍可正常运行，只是对应音轨不会发声。

## 审计结论

审计日期：`2026-04-08`

本次对 `src/audio_system.cpp`、`src/game_session.cpp`、`src/game_play.c`、`src/game_overlay.cpp` 的交叉检查结果如下：

- 当前代码共引用 `26` 个运行时音频文件：`19` 个 `wav` 音效 + `7` 个 `bgm_*_1.ogg`
- 当前代码引用的音频文件在本次审计时已全部存在
- 目录中额外存在 `7` 个 `bgm_*_2.ogg` 备用版本，但当前代码尚未接线
- 听觉层面的主要缺口已经从“BGM 缺失”变成“第二套 BGM 变体尚未启用”
- 随着地图重构和剧情扩展方向明确，新的音频缺口将更多落在“新区域辨识度”和“剧情节点短演出音”上

另外还有两条运行时规则需要明确：

- `masterVolume` 会作用于整体输出，默认值为 `0.80`
- `sfxEnabled` 只会屏蔽 `Audio_PlayCue()` 触发的音效，不会关闭 `Audio_SetScene()` 驱动的 BGM 场景切换逻辑

## 当前代码已经主动加载的音频

以下文件都会由 `src/audio_system.cpp` 主动尝试加载。

### 界面与交互音效（8）

- `ui_confirm.wav`
- `ui_open.wav`
- `ui_close.wav`
- `ui_warning.wav`
- `ui_craft.wav`
- `ui_collect.wav`
- `ui_log.wav`
- `ui_repair.wav`

### 战斗、环境与脚步音效（8）

- `combat_hurt.wav`
- `combat_melee.wav`
- `combat_laser.wav`
- `monster_roar.wav`
- `ambient_monolith.wav`
- `step_forest.wav`
- `step_metal.wav`
- `step_swamp.wav`

### 结局音效（3）

- `ending_heroic.wav`
- `ending_peaceful.wav`
- `ending_settlement.wav`

以上 `19` 个 `wav` 文件在本次审计时均已存在，不会触发静音回退。

### 场景背景音乐（7 条已接线 + 7 条备用）

当前代码已经改为加载以下循环 BGM 主版本：

- `bgm_menu_1.ogg`
- `bgm_base_1.ogg`
- `bgm_forest_1.ogg`
- `bgm_swamp_1.ogg`
- `bgm_ruins_1.ogg`
- `bgm_boss_1.ogg`
- `bgm_endings_1.ogg`

以上 `7` 个 `ogg` 文件在本次审计时均已存在，会作为当前运行版本实际播放的场景音乐。

目录中还额外存在以下备用版本，但当前代码不会主动加载：

- `bgm_menu_2.ogg`
- `bgm_base_2.ogg`
- `bgm_forest_2.ogg`
- `bgm_swamp_2.ogg`
- `bgm_ruins_2.ogg`
- `bgm_boss_2.ogg`
- `bgm_endings_2.ogg`

## 当前接线路径

- `src/audio_system.cpp`
  负责可选加载、场景音乐切换、音效播发、音量设置与静音回退
- `src/game_session.cpp`
  负责菜单 / 载入 / 开局过程中的场景音乐切换，以及部分确认 / 警告 / 打开提示音
- `src/game_play.c`
  负责按区域切换 `base / forest / swamp / ruins / boss / ending` 场景音乐，并触发脚步、采集、日志、修复、战斗、石碑与结局音效
- `src/game_overlay.cpp`
  负责背包、地图、设置、暂停、制作、定居确认等覆盖层的打开 / 关闭 / 确认 / 制作提示音

## 当前已覆盖的行为

当前音效已经明确覆盖以下体验节点：

- 菜单、面板、背包、地图、设置、暂停等界面的打开 / 关闭 / 确认
- 制作、采集、日志拾取、日志面板打开、修复类关键交互
- 森林 / 舱内金属地面 / 沼泽三类脚步声
- 近战、激光、受击、怪物警戒、石碑环境反馈
- 英雄、和平、定居三类成功结局音效
- 菜单、基地、森林、沼泽、遗迹、Boss、终局七种 BGM 场景切换逻辑
- 每个场景当前固定使用 `_1` 版本；`_2` 版本仅作资源备用

## 当前缺口

当前最主要的待办，不再是补齐 BGM 文件，而是决定第二套 BGM 变体的启用策略，例如：

1. 保持 `_1` 固定播放，`_2` 长期仅作备用
2. 进入场景时随机选择 `_1 / _2`
3. 按区域状态、昼夜或剧情阶段切换 `_1 / _2`
4. 在设置中加入“标准 / 备用配乐”选择

在此之前，当前版本的实际行为是：每个场景稳定播放 `_1`，`_2` 只存在于资源目录，不参与运行时逻辑。

## 按新地图设计需要补充的音频方向

以下内容建议作为下一轮资源规划，不视为当前已接线。

### A. 飞船内部空间辨识音

建议新增：

- `ambient_ship_corridor.wav`
- `ambient_medbay_hum.wav`
- `ambient_support_station.wav`
- `ambient_storage_rattle.wav`

用途：

- 区分飞船主通道、医疗舱、支援舱、储藏舱的空间感
- 让飞船内部不只靠视觉区分功能

### B. 北侧主线区环境音

建议新增：

- `ambient_ruins_north_wind.wav`
- `ambient_monolith_active.wav`
- `ambient_signal_tower_charge.wav`
- `ambient_ruins_vent.wav`

用途：

- 强化北侧石碑 / 遗迹 / 信号塔区域的终局感
- 让玩家一靠近关键主线装置就感到氛围变化

### C. 西线 / 南线第二轮剧情音色（未接线规划）

建议新增：

- `ambient_echo_wilds.wav`
- `ambient_subsurface_sink.wav`
- `ambient_subsurface_machine.wav`

用途：

- 为西线 / 南线第二轮剧情建立独立听觉身份
- 避免所有新区域都继续复用 swamp 或 ruins 音景

## 剧情演出音频规划（未接线）

随着开场分镜和后续关键节点剧情图的设计补充，音频层也可以预留一组“剧情演出资源”，但当前仍处于规划状态，尚未计入已接线资源：

- 开场分镜专用短氛围音 / 转场提示音
- 氧气修复、通讯修复、能源恢复等关键节点的短剧情提示音
- 洛希（Loxi）终端同步、遗迹碎片分析、石碑共鸣等叙事节点的独立音色
- 英雄救援 / 和平救援 / 定居 / 失败结局的分镜垫底音

建议后续如果正式接线，再单独采用明确命名，例如：

- `story_intro_stinger.wav`
- `story_oxygen_restore.wav`
- `story_loxi_sync.wav`
- `story_monolith_resonance.wav`
- `story_signal_tower.wav`

在正式接线前，这些文件不应写入“当前代码已经主动加载的音频”列表。

## 建议规格

- 短音效继续使用 `wav`
- 循环背景音乐继续使用 `ogg`
- 采样率建议统一在 `44.1kHz` 或 `48kHz`
- 不同资源的响度基准尽量统一，避免 UI 音、战斗音、结局音突然跳响
- BGM 最好直接做成可无缝循环的头尾，减少 `Audio_SetScene()` 切换后的突兀感

## 注意事项

- 文件名保持 ASCII，并严格区分大小写
- 不要放置空白占位音频；缺文件时当前系统已经会自动静音回退
- 现在 BGM 文件名已经从旧的 `bgm_*.ogg` 改为 `bgm_*_1.ogg / bgm_*_2.ogg`
- 只把文件放进目录不代表游戏一定会播，仍需同步确认 `src/audio_system.cpp` 和调用侧是否真的接线
