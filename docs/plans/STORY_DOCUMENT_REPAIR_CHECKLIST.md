# SpaceCraft Living 剧情文档修正清单（2026-04-17）

## 1. 文档目的

本文档用于把 `LONGFORM_STORY_DRAFT.md`、活动剧情文档、资源文档和当前代码口径重新对齐，避免“写作目标”和“当前运行时事实”混写。

## 2. 本轮修正范围

### 已完成

- [x] 明确长篇剧情稿是“写作母稿”，不是当前运行时实现清单。
- [x] 把 `26` 份日志目标标记为“后续扩写目标”，不再与当前 `14` 份已接线日志混写。
- [x] 明确 `Loxi Synthesis` 当前是写作层 / 表现层预留，不是现有 UI 中的第三种正式档案分类。
- [x] 统一长篇稿中的日志标题写法，尽量对齐当前运行时正式标题前缀：`Ship Log`、`Field Record`、`Facility Record`、`Crash Recorder`。
- [x] 修正 `Action Declaration Record` 的身份：它是玩家终局前的归档动作，不再和 `D12` 船员前史记录混写。
- [x] 修正 `WEST_SOUTH_FULL_STORY.md` 中过期的“仅 W1-W3 / S1-S3 可跑”状态描述。
- [x] 修正 `WEST_SOUTH_RESOURCE_GAP.md` 中过期的资源缺口快照，使其不再与当前资源审计冲突。
- [x] 将新剧情母稿与修正清单纳入文档入口，方便后续继续维护。
- [x] 回填洛希终端里的日志详细描述正文，使当前运行时 14 份已接线日志可以直接对应写作层内容。

### 后续可继续做

- [ ] 把长篇稿中的第一批长文母稿继续拆成“运行时短版日志 / 基地总结 / 结局引用段”。
- [ ] 为 `X1-X3` 增加单独的终局前文本规范表，约束 objective、communicator、ending panel 的一致写法。
- [ ] 在代码层为“玩家亲手整理真相”的动作补正式交互和 smoke 测试。
- [ ] 继续把后续扩写目标整理成可直接回填的运行时短版日志，避免再次和当前 `14` 份已接线日志混写。

## 3. 当前统一口径

- 当前运行时代码已接线并实际可玩的日志数量是 `14` 份。
- 当前运行时这 `14` 份日志的 `detailText` 已经接入洛希终端详情流。
- 当前正式 UI 分类仍然只有：
  - `Main Archive`
  - `Supplemental Archive`
- `Loxi Synthesis` 当前应理解为：
  - 基地总结
  - 终局前重写摘要
  - 写作层预留表现
  而不是当前已上线的第三种日志分类页。
- 长篇稿中的 `26` 份日志方案是“文档扩写目标”，不是“当前游戏里已经可收集的 26 份档案”。
- 北线新增档案和终局宣言文本，在正式接线前不得默认写成 `Stage 7` 的当前硬条件。

## 4. 建议阅读顺序

1. [`/Users/jason/Documents/SpaceCraftLivng/docs/design/GAME_DESIGN.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/GAME_DESIGN.md)
2. [`/Users/jason/Documents/SpaceCraftLivng/docs/design/CURRENT_STATUS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/CURRENT_STATUS.md)
3. [`/Users/jason/Documents/SpaceCraftLivng/docs/design/STAGE_FLOW_CODE_VERIFIED.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/STAGE_FLOW_CODE_VERIFIED.md)
4. [`/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_STORY_TASKS.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/EXPANSION_STORY_TASKS.md)
5. [`/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md)
6. [`/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/LONGFORM_STORY_DRAFT.md)
7. 本文档

## 5. 使用规则

- 如果问题是“当前代码已经实现了什么”，优先以 `CURRENT_STATUS.md` 和 `STAGE_FLOW_CODE_VERIFIED.md` 为准。
- 如果问题是“当前活动设计要求什么”，优先以 `GAME_DESIGN.md`、`EXPANSION_STORY_TASKS.md`、`NEXT_STEPS.md` 为准。
- 如果问题是“后续剧情要扩写成什么厚度”，再看 `LONGFORM_STORY_DRAFT.md`。
- 如果长篇稿与当前运行时事实冲突，以当前代码和活动文档为准，再回改长篇稿。
