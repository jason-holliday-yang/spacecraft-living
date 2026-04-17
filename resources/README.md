# SpaceCraft Living 资源目录说明

资源文档统一按两个维度维护：

- `资源媒介`
  图片 / 音频 / 视频
- `运行时状态`
  已接线且完整 / 已接线但缺文件 / 已入库未接线 / 仅规划未接线

本文档只记录“当前工作区真实状态”，不把历史规划误记成已完成。

## 当前快照

审计日期：`2026-04-15`

- 图片资源：
  当前代码引用 `119` 张 PNG，目录中现有 `119` 张 PNG；当前图片资源已全部接线并完整存在。
- 叙事图片：
  当前已接线并存在 `48` 张，细分为 `5` 张开场分镜、`22` 张主线剧情图、`14` 张日志剧情图、`7` 张结局背景图。
- 地图 / 角色 / UI / 敌人图片：
  当前已接线并存在 `71` 张，覆盖角色、地图地块、场景物件、资源节点、制作图标、状态图标、怪物与 Boss。
- 音频资源：
  当前代码引用 `26` 个运行时音频文件，`26/26` 全部存在。
- 视频资源：
  当前仍未接入播放链路，`resources/videos/` 继续作为规划目录维护。

## 按媒介分类

### `resources/images/`

用途：

- 开场分镜
- 主线 / 日志 / 结局剧情卡
- 地图地表与物件可读性补图
- 资源节点、装备图标、怪物、角色贴图

当前状态：

- 已接线且存在：`119`
- 已接线但缺文件：`0`
- 已入库但未接线：`0`

详细分类见：

- [`images/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/images/README.md)

### `resources/audio/`

用途：

- UI 交互音
- 战斗与环境音
- 场景 BGM
- 结局音效

当前状态：

- 已接线且存在：`26`
- 备用未启用：`7` 个 `bgm_*_2.ogg`
- 已接线但缺文件：`0`

详细说明见：

- [`audio/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/audio/README.md)

### `resources/videos/`

用途：

- 未来开场 / 章节 / 终局短演出

当前状态：

- 仅规划未接线

详细说明见：

- [`videos/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/videos/README.md)

## 按运行时状态分类

### 已接线且完整

- 图片：
  开场分镜、`22` 张主线图、`14` 张日志图、`7` 张结局图、角色贴图、地图主地表、barrier 地块、资源节点、制作图标、状态图标、怪物贴图都已进入运行时加载链路。
- 音频：
  当前 `wav` 音效和 `bgm_*_1.ogg` 主版本全部存在并可加载。

### 已接线但缺文件

- 当前为 `0`

### 已入库未接线

- 当前为 `0`

### 仅规划未接线

- 视频目录整体仍为规划态
- 音频目录中的 `bgm_*_2.ogg` 仍为备用版本

## 按内容职责分类

### 叙事演出

- `cutscenes/`
- `story/main/`
- `story/logs/`
- `story/endings/`

当前职责：

- 开场说明坠毁背景
- 主线关键节点收束阶段推进
- 日志图承担“信息推进”而不是奖励插画
- 结局图承担路线复盘和终幕氛围

### 地图与空间可读性

- 飞船主通道 / 舱室地表
- 外沼泽 / 深沼泽地表
- 遗迹入口 / 环区 / 塔顶平台地表
- Echo Basin 地表
- 控制台、石碑、信号塔、坠毁线索等关键物件

### 系统与战斗可读性

- 角色、资源节点、怪物、Boss
- 制作图标与背包条目图
- 状态图标已经接入 HUD 状态栏

## 维护规则

- 新资源先判断是否真的有代码加载入口，再记为“已接线”。
- 如果只是文件落盘但没有运行时引用，必须记为“已入库未接线”。
- 如果代码已经引用但文件缺失，必须记到“已接线但缺文件”。
- 文档中的数量应以当前工作区为准，而不是沿用旧审计数字。
- 资源命名继续保持 ASCII、小写、下划线分词。

## 关联文档

- [`images/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/images/README.md)
- [`audio/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/audio/README.md)
- [`videos/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/videos/README.md)
