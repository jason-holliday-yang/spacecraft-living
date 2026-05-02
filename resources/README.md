# SpaceCraft Living 资源目录说明

资源文档统一按两个维度维护：

- `资源媒介`
  图片 / 音频 / 视频
- `运行时状态`
  已接线且完整 / 已接线但缺文件 / 已入库未接线 / 仅规划未接线

本文档只记录“当前工作区真实状态”，不把历史规划误记成已完成。

## 当前快照

审计日期：`2026-04-26`

- 图片资源：
  当前代码引用 `146` 张 PNG，目录中现有 `151` 张 PNG；当前图片运行时引用路径 `146 / 146` 全部完整存在，额外 `5` 张是主线正式命名兼容文件。
- 叙事图片：
  当前叙事运行时槽位共 `56` 张，细分为 `5` 张开场分镜、`30` 张主线剧情图、`14` 张日志剧情图、`7` 张结局背景图；另保留 `22` 张历史主线兼容文件。
- 玩法 / 地图 / 角色 / UI / 敌人图片：
  当前已接线并存在 `71` 张，已经按 `characters / world / ui / enemies` 四类路径收口。
- 音频资源：
  当前代码主动加载 `33` 个运行时音频文件：`19` 个 `wav` 音效 + `14` 个 `ogg` 音乐文件；当前 `33/33` 全部存在。资源目录另有 `10` 个阶段 / 路线 BGM 已入库但尚未接线。
- 视频资源：
  当前仍未接入播放链路，`resources/videos/` 继续作为规划目录维护。
  当前已在视频资源文档中标记第一批 `5` 段“可能加入”的主线短演出资源位。

## 当前目录分层

### `resources/images/`

当前已按职责归档到以下路径：

- `resources/images/story/`
  叙事演出图，包含 `cutscenes / main / logs / endings`
- `resources/images/characters/`
  玩家与 NPC 贴图，包含 `player / npc`
- `resources/images/world/`
  地图地表、场景物件、barrier 和采集节点，包含 `tiles / nodes`
- `resources/images/ui/`
  HUD / 背包 / 制作 / 状态栏相关图片，包含 `icons / status`
- `resources/images/enemies/`
  常规怪与 Boss 贴图

详细分类见：

- [`images/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/images/README.md)

### `resources/audio/`

当前已按职责归档到以下路径：

- `resources/audio/sfx/ui/`
  面板、确认、日志、修复、采集等 UI / 交互音效
- `resources/audio/sfx/combat/`
  受击、近战、激光、怪物提示音
- `resources/audio/sfx/ambient/`
  场景环境提示音
- `resources/audio/sfx/footsteps/`
  森林 / 金属 / 沼泽脚步声
- `resources/audio/sfx/endings/`
  结局成功路线音效
- `resources/audio/music/`
  当前已接线场景音乐拆分为 `menu / base / forest / swamp / ruins / boss / endings`；新阶段 / 路线音乐已落在 `stages / routes`，尚未接线

详细说明见：

- [`audio/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/audio/README.md)

## 命名与剧情对齐规则

- 图片叙事图统一按剧情层级命名：
  - `story/cutscenes/intro_##_<scene_slug>.png` -> 开场分镜
  - `story/main/story_main_m##_<scene_slug>_v001.png` -> 主线剧情卡 / 区域推进卡正式文件名
  - `story/logs/log_##_<scene_slug>.png` -> 已接线日志详情图
  - `story/endings/ending_##_<scene_slug>.png` -> 结局背景图
- 历史主线兼容文件仍保留在 `story/main/main_##_<scene_slug>.png`，但不再作为正式命名口径。
- 当前剧情口径下：
  - `log_01-log_03` = 飞船起始档案
  - `log_04-log_08` = 西线档案
  - `log_09` = 东线坠毁异常记录
  - `log_10-log_14` = 南线设施档案
  - `main_14-main_16` = 西线 `W4-W5`
  - `main_17-main_19` = 南线 `S4-S5`
  - `main_20-main_22` = 跨区联动 `X1-X3` / 终局前总结
- 音频命名统一按运行时职责命名：
  - `sfx/ui/*.wav` = 界面 / 交互提示
  - `sfx/combat/*.wav` = 战斗 / 怪物提示
  - `sfx/ambient/*.wav` = 场景环境反馈
  - `sfx/footsteps/*.wav` = 地表脚步声
  - `music/<scene_group>/bgm_<scene_group>_<variant>.ogg` = 当前已接线场景 BGM 变体
  - `music/stages/bgm_stage<stage>_<slug>_<variant>.ogg` = 计划中的阶段 BGM 变体
  - `music/routes/bgm_route_<route>_<variant>.ogg` = 计划中的路线 BGM 变体
- UI 功能 / 按键提示图标继续使用 `resources/images/ui/icons/icon_<subject>_button.png` 口径，为“功能身份”保留明确后缀，避免和物品 / 装备 icon 混名。
- 当前音频与地图 / 剧情口径对齐为：
  - 西线 `West Frontier -> Last Camp` 统一走 `music/forest/`
  - 东线外沼与深沼分别走 `music/swamp/` 两个变体
  - 北线遗迹与南线设施共用 `music/ruins/` 两组变体，但南线固定落在 `ruins facility` 语义
  - 英雄路线预战区与隐藏 boss arena 走 `music/boss/`
  - 结局页统一走 `music/endings/`，三条成功路线额外叠加各自 `sfx/endings/` 结局音效
- 下一轮音频播放目标为“阶段音乐主控”：普通探索不再由地图位置直接抢主 BGM，具体执行见 [`../docs/plans/STAGE_MUSIC_PLAYBACK_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/STAGE_MUSIC_PLAYBACK_PLAN.md)。

### `resources/videos/`

用途：

- 未来开场 / 章节 / 终局短演出

当前状态：

- 仅规划未接线
- 已标记第一批“可能加入”的主线短演出资源位：
  `M04`、`M17`、`M19`、`M24`、`M29`

详细说明见：

- [`videos/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/videos/README.md)

## 按运行时状态分类

### 已接线且完整

- 图片：
  `story`、`characters`、`world`、`ui`、`enemies` 下的当前 PNG 全部已经进入运行时加载链路。
- 音频：
  `sfx` 和 `music` 下的当前 `33` 个运行时文件全部存在并可加载。

### 已接线但缺文件

- 当前为 `0`

### 已入库未接线

- 音频：
  `resources/audio/music/stages/` 与 `resources/audio/music/routes/` 下的 `10` 个阶段 / 路线 BGM 已入库，等待按 [`STAGE_MUSIC_PLAYBACK_PLAN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/plans/STAGE_MUSIC_PLAYBACK_PLAN.md) 接线。

### 仅规划未接线

- 视频目录整体仍为规划态

## 维护规则

- 新资源必须先决定归属目录，再落盘：
  - 叙事图进 `resources/images/story/...`
  - 角色图进 `resources/images/characters/...`
  - 地图 / 交互物进 `resources/images/world/...`
  - UI 图进 `resources/images/ui/...`
  - 敌人图进 `resources/images/enemies/`
  - 音效进 `resources/audio/sfx/...`
  - 音乐进 `resources/audio/music/...`
- 新资源先判断是否真的有代码加载入口，再记为“已接线”。
- 如果只是文件落盘但没有运行时引用，必须记为“已入库未接线”。
- 如果代码已经引用但文件缺失，必须记到“已接线但缺文件”。
- 文档中的数量应以当前工作区为准，而不是沿用旧审计数字。
- 资源命名继续保持 ASCII、小写、下划线分词。

## 关联文档

- [`images/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/images/README.md)
- [`audio/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/audio/README.md)
- [`videos/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/videos/README.md)
