# SpaceCraft Living 叙事运行时替换计划（2026-04-27）

> 当前状态说明：本文档不是新的剧情母稿，而是把现有运行时代码里的文本来源，与 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md) 中已经整理好的主线卡、日志和结局扩写稿一一对应起来的执行文档。它只回答一个问题：`如果现在开始把更完整的故事真正写回游戏，应当先改哪里、怎么分批改、每处文本应替换成母稿中的哪一层。`

## 1. 文档目的

当前剧情母稿已经足够完整：

- 世界背景、人物、时间线已经统一
- `30` 段主线剧情卡已经写完短版 / 长版 / 洛希摘要
- `14` 份已接线日志已经写完短版 / 长版 / 洛希摘要
- `4` 个正式结局已经写完短版 / 长版 / 结案摘要

现在缺的不是继续扩写，而是一个实际可执行的替换计划。

本文档的职责是：

1. 指出当前运行时文本真正定义在哪些文件、哪些符号里。
2. 把每一层运行时文本，对应回剧情母稿中的具体章节。
3. 给出分批替换顺序，避免一次性改动过大而失控。
4. 给后续真正开始改代码的人一份可以照着做的清单。

## 2. 使用范围

本文档当前只覆盖三类运行时文本：

- 主线剧情卡
- 日志文本
- 结局文本

当前不覆盖：

- 任务 Objective 短句
- 地图提示文案
- 帮助页 / 新手引导
- 纯系统性状态提示

这些文本后续可以继续补，但不应和本轮叙事替换混在一起推进。

## 3. 当前运行时文本源

### 3.1 主线剧情卡

主线剧情卡当前的主要文本源如下：

- [ui_narrative_panels.cpp](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp:57)
  `kStoryMainDefs`
  用于主线剧情卡首屏标题与正文
- [ui_narrative_panels.cpp](/Users/jason/Documents/SpaceCraftLivng/src/ui_narrative_panels.cpp:90)
  `kStoryMainDetailTexts`
  用于主线故事页 detailText

结论：

- `kStoryMainDefs[].body` 应替换为母稿中的 `短版`
- `kStoryMainDetailTexts[]` 应替换为母稿中的 `长版`

### 3.2 日志文本

日志当前主要通过种子数据写入任务运行时：

- [task_content_data.cpp](/Users/jason/Documents/SpaceCraftLivng/src/task_content_data.cpp:93)
  `kLogSeeds`
  每条日志包含：
  - `title`
  - `story`
  - `detailText`
- [task_seed_setup.c](/Users/jason/Documents/SpaceCraftLivng/src/task_seed_setup.c:66)
  `titleEn/titleZh`
  `storyTextEn/storyTextZh`
  `detailTextEn/detailTextZh`

结论：

- `storyText*` 应替换为母稿中的日志 `短版`
- `detailText*` 应替换为母稿中的日志 `长版`

### 3.3 结局文本

结局文本当前主要定义在：

- [task_content.cpp](/Users/jason/Documents/SpaceCraftLivng/src/task_content.cpp:31)
  `kEndingTextDefs`
  用于结局标题与正文
- [task_content.cpp](/Users/jason/Documents/SpaceCraftLivng/src/task_content.cpp:1517)
  `TasksContent_GetEndingTitleText`
- [task_content.cpp](/Users/jason/Documents/SpaceCraftLivng/src/task_content.cpp:1528)
  `TasksContent_GetEndingBodyText`

结论：

- `kEndingTextDefs[].body` 应替换为母稿中的结局 `短版` 或经过压缩的首屏版
- 若后续结局页支持更长正文，应优先把 `长版` 接入单独正文层，而不是继续塞在现有短正文接口里

### 3.4 当前尚未独立接线但应预留的层

这些层目前还没有清晰的专属文本源，但应在替换计划中预留：

- 主线阶段后的 `洛希归档摘要`
- 结局后的 `洛希结案摘要`

建议后续接线位置：

- 通讯器 Story / Logs 页新增或复用摘要块
- 结局页后附一段结案归档

## 4. 剧情母稿对应章节

运行时替换时，不要从母稿里手动挑句子，应直接按以下章节来源取文。

### 4.1 主线剧情卡来源

- `M01-M10`
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:1599)
  `23. 主线剧情卡扩写初稿 M01-M10`
- `M11-M20`
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:1724)
  `24. 主线剧情卡扩写初稿 M11-M20`
- `M21-M30`
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:1858)
  `25. 主线剧情卡扩写初稿 M21-M30`

替换规则：

- `短版` -> 首屏剧情卡
- `长版` -> detailText
- `洛希归档摘要` -> 后续新接线摘要层

### 4.2 日志来源

- `L01-L05`
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:2000)
  `26. 日志正式扩写初稿 L01-L05`
- `L06-L10`
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:2107)
  `27. 日志正式扩写初稿 L06-L10`
- `L11-L14`
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:2217)
  `28. 日志正式扩写初稿 L11-L14`

替换规则：

- `短版` -> `storyText*`
- `长版` -> `detailText*`
- `洛希归档摘要` -> 后续回船总结或日志归档层

### 4.3 结局来源

- `4` 个结局
  见 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md:2307)
  `29. 结局正文扩写初稿`

替换规则：

- `短版` -> 当前结局页首屏正文
- `长版` -> 结局长页或未来分页正文
- `洛希结案摘要` -> 结算补充摘要

## 5. 主线卡替换清单

### 5.1 直接替换项

需要直接替换的现有主线卡文本：

- `M01-M30` 对应的 `kStoryMainDefs[].body`
- `M01-M30` 对应的 `kStoryMainDetailTexts[]`

替换方式：

- 不改标题编号与标题结构
- 只替换正文

保留不动：

- `eyebrow`
- `title`
- 色彩 / 背景 / 纹理相关定义

### 5.2 替换时的注意事项

- 首屏 `短版` 不能被再压成一句更短 slogan
- `长版` 若因 UI 行数限制需要缩减，优先删一层修辞，不删因果与意义段
- 中英双语必须同步更新，不能先只改英文

### 5.3 主线卡批次建议

第一批：

- `M01-M10`

第二批：

- `M11-M20`

第三批：

- `M21-M30`

理由：

- 第一批最直接决定“开头是否还薄”
- 第二批最直接决定“西线 / 南线是否有重量”
- 第三批最直接决定“终局前是否终于站稳”

## 6. 日志替换清单

### 6.1 直接替换项

需要直接替换的现有日志字段：

- `title`
  原则上本轮不改，除非后续需要统一命名口径
- `story`
  替换为母稿 `短版`
- `detailText`
  替换为母稿 `长版`

### 6.2 当前 14 份日志对应顺序

建议保持现有运行时顺序不变，只替换内容：

1. `Impact Protocol`
2. `Split Roster`
3. `Pattern, Not Wilderness`
4. `West Signal Fragment 01`
5. `Survey Break Anchor Notes`
6. `Canopy Handoff Record`
7. `Echo Basin Topology Sketch`
8. `Last Camp Testament`
9. `Black Box Residue`
10. `Purifier Outage Memo`
11. `Vent Calibration Handover`
12. `Service Shaft Sync Record`
13. `Purifier Ring Control Brief`
14. `Root Vault Core Dossier`

### 6.3 日志批次建议

第一批：

- `L01-L05`

第二批：

- `L06-L10`

第三批：

- `L11-L14`

理由：

- 第一批决定事故起点与西线入口是否立住
- 第二批决定人物接力与事故闭环是否立住
- 第三批决定系统真相与终局前提是否立住

## 7. 结局替换清单

### 7.1 直接替换项

当前立即可替换：

- `ENDING_HEROIC`
- `ENDING_PEACEFUL`
- `ENDING_SETTLEMENT`
- `ENDING_FAILURE`
  对应 `kEndingTextDefs[].body`

### 7.2 结局页结构建议

如果不改 UI 结构：

- 先只接 `短版`

如果允许结局页多一层正文：

- 首屏接 `短版`
- 详情或分页接 `长版`

如果允许结算补充页：

- 再接 `洛希结案摘要`

### 7.3 结局替换顺序

建议和主线 / 日志不同，结局一次改完：

- 英雄
- 和平
- 定居
- 失败

理由：

- 四条结局是同一套伦理框架的不同答案
- 分开逐条替换会导致一段时间内结局层风格断裂

## 8. 洛希摘要接线建议

这一层目前没有明确独立数据结构，但它对“剧情终于完整”非常关键。

### 8.1 推荐新增的两类摘要

- `阶段归档摘要`
  来源：主线卡 `洛希归档摘要`
- `结案归档摘要`
  来源：结局 `洛希结案摘要`

### 8.2 推荐显示位置

优先级更高的显示位置：

- 通讯器 Story 页详情面板下方追加摘要块
- 结局正文后追加“洛希结案”

如果当前 UI 不好加：

- 先建数据结构
- UI 层后补

### 8.3 为什么这一层值得单独做

因为它承担的是：

- 把玩家刚读到的碎片重新解释成主线
- 明确展示洛希自己也在修正判断
- 让“回船确认路线”真正有叙事意义

如果只替换主线卡和日志，不补这一层，故事会更厚，但“被重新解释”的感觉仍会不够强。

## 9. 首轮实施优先级

### 9.1 P1：最先替换

- `M01-M10` 主线卡首屏与 detailText
- `L01-L05` 日志 `story/detail`
- `L14` 根脉核心档案
- `4` 条结局短版

目标：

- 立刻改善开头、事故真相和结局重量

### 9.2 P2：第二轮替换

- `M11-M20`
- `L06-L10`

目标：

- 立刻改善西线人物层与南线系统层

### 9.3 P3：第三轮替换

- `M21-M30`
- `L11-L13`
- 结局长版

目标：

- 让终局前的立场收束真正完整

### 9.4 P4：新增层

- 洛希阶段归档摘要
- 洛希结案摘要

目标：

- 让“回船确认路线”和“结局后归档”不再只是功能流程，而是叙事闭环

## 10. 执行建议

后续真正开始改代码时，建议严格按下面顺序执行：

1. 先改剧情卡正文
2. 再改日志 `story/detail`
3. 再改结局正文
4. 最后新增洛希摘要层

不要建议的顺序：

- 先动结局，再动前文
- 中英只改一边
- 同一批里同时改剧情和 UI 布局

原因很简单：

- 先把文本本身替掉，才能验证新叙事是否站得住
- UI 层变化应和文本层变化拆开，避免一次性变量过多

## 11. 当前结论

当前最重要的判断是：

- 不需要再写新的平行剧情稿
- 不需要再继续发散更多设定稿
- 下一步如果要进入真正落地，应以本文档为执行入口，以 [`LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md) 为唯一内容来源

一句话总结：

- `剧情母稿已经够了；接下来该做的是有顺序地把它写回运行时。`
