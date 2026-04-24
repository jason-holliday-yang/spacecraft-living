# SpaceCraft Living 西线 / 南线资源缺口清单（2026-04-17）

## 1. 文档目的

本文档当前用于回答两个问题：

- 在当前 `W1-W5`、`S1-S5` 与 `X1-X3` 已接线的前提下，哪些资源属于“当前已完成基线”，哪些属于“后续剧情扩写或演出增强”的候选项。
- 为后续演出增强，哪些视频资源值得准备，以及当前是否已具备播放接线。

配套文档：

- 图片总审计：[`../../resources/images/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/images/README.md)
- 资源目录总览：[`../../resources/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/README.md)
- 完整剧情稿：[`WEST_SOUTH_FULL_STORY.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md)

## 2. 当前资产状态快照

- 已接线图片：当前代码已引用 `119` 张 PNG，当前 `119/119` 全部存在。
- 其中叙事图片：`48` 张；玩法 / 地图 / 角色 / UI / 敌人图片：`71` 张。
- 已接线音频：当前代码主动加载 `33` 个运行时音频文件，当前 `33/33` 全部存在。
- 视频播放接线：当前代码未检出 `mp4/webm/video` 播放链路。

说明：

- 当前活动版本已经不存在“已接线但缺图”的西线 / 南线硬缺口。
- 本文档下面列出的图片与视频，更适合作为“后续剧情厚化 / 演出增强 / 长篇稿扩写”资源池，而不是再被表述成“当前缺失导致功能不完整”的必补项。
- 视频资产可先进入规划池，待播放系统接线后再转生产优先级。

## 3. 图片资源新增清单

### P0：当前基线已完成（记录用）

此前地图可读性贴图缺口已经补齐；以下类型当前都已在工作区存在并完成接线：

- 飞船走廊 / 舱室地表
- 外沼 / 深沼地表
- 遗迹路径 / 石碑环区 / 塔顶平台地表
- swamp / deep / ruins barrier 地块

当前如需继续新增图片，应理解为“提升表现层密度”，而不是“补当前缺图”。

### P1：西线第二轮剧情图（W4-W5）

对应剧情节点：`Echo Basin` 与 `Last Camp`。

当前已存在并作为命名基线的叙事图：

- `story/main/main_14_echo_basin_lock.png`
  对应 `W4` 回声定位成功瞬间。
- `story/main/main_15_loxi_route_rewrite.png`
  对应 `W4` 结束后洛希策略重写。
- `story/main/main_16_last_camp_archive.png`
  对应 `W5` 最后营地档案回收。
- `story/logs/log_04_west_signal_fragment_01.png`
  对应《Field Record: West Signal Fragment 01》。
- `story/logs/log_05_survey_break_anchor_notes.png`
  对应《Field Record: Survey Break Anchor Notes》。
- `story/logs/log_06_canopy_handoff_record.png`
  对应《Field Record: Canopy Handoff Record》。
- `story/logs/log_07_echo_basin_topology_sketch.png`
  对应《Field Record: Echo Basin Topology Sketch》。
- `story/logs/log_08_last_camp_testament.png`
  对应《Field Record: Last Camp Testament》。

如果继续提高地图读感，当前西线已存在的基础地表是：

- `tile_echo_basin_floor.png`

在此基础上，优先新增的仍是场景补图：

- `tile_echo_beacon_array.png`
- `tile_last_camp_tent.png`
- `tile_last_camp_archive_box.png`
- `tile_west_observation_post.png`

### P1：南线第二轮剧情图（S4-S5）

对应剧情节点：`Purifier Ring` 与 `Root Vault`。

当前已存在并作为命名基线的叙事图：

- `story/main/main_17_purifier_ring_boot.png`
  对应 `S4` 净化环重启。
- `story/main/main_18_global_risk_drop.png`
  对应 `S4` 风险结构被改写。
- `story/main/main_19_root_vault_core.png`
  对应 `S5` 根系密库核心记录下载。
- `story/logs/log_09_crash_recorder_black_box_residue.png`
  对应《Crash Recorder: Black Box Residue》；它本身是东线异常记录，但会在南线系统真相链里被再次引用。
- `story/logs/log_10_purifier_outage_memo.png`
  对应《Facility Record: Purifier Outage Memo》。
- `story/logs/log_11_vent_calibration_handover.png`
  对应《Facility Record: Vent Calibration Handover》。
- `story/logs/log_12_service_shaft_sync_record.png`
  对应《Facility Record: Service Shaft Sync Record》。
- `story/logs/log_13_purifier_ring_control_brief.png`
  对应《Facility Record: Purifier Ring Control Brief》。
- `story/logs/log_14_root_vault_core_dossier.png`
  对应《Facility Record: Root Vault Core Dossier》。

如果继续提高地图读感，优先新增的仍是场景补图：

- `tile_purifier_ring_core.png`
- `tile_purifier_console.png`
- `tile_root_vault_gate.png`
- `tile_root_vault_archive.png`
- `tile_subsurface_cooling_pipe.png`

### P2：跨区联动与终局反哺图（X1-X3）

当前已存在并作为命名基线的叙事图：

- `story/main/main_20_trace_correlation.png`
  对应 `X1` 痕迹对照。
- `story/main/main_21_loxi_sync_rewrite.png`
  对应 `X2` 策略重写。
- `story/main/main_22_final_stance.png`
  对应 `X3` 终局前立场确认。
- `story/endings/ending_05_heroic_with_records.png`
- `story/endings/ending_06_peaceful_with_repair.png`
- `story/endings/ending_07_settlement_with_legacy.png`

如果后续继续加厚终局表现，优先新增的应是：

- 结局页专用局部特写图，而不是重新改写现有 `ending_05-ending_07` 的命名体系
- 与 `X1-X3` 对应的辅助场景 prop 图，保持 `world/tiles/props/` 归档，不要把环境补图混进 `story/`

## 4. 视频资源规划清单

当前定位：

- 视频不是当前运行时必需资产。
- 先做资源规划，不进入“已接线资源”统计。

### 推荐视频清单（规划态）

- `videos/intro/intro_crash_recap_15s.mp4`
  用途：新档开场前 15 秒坠毁回顾。
- `videos/west/w4_echo_lock_12s.mp4`
  用途：`W4` 回声定位成功短演出。
- `videos/west/w5_last_camp_memory_12s.mp4`
  用途：`W5` 最后营地记忆拼接。
- `videos/south/s4_purifier_boot_12s.mp4`
  用途：`S4` 净化环重启。
- `videos/south/s5_root_vault_reveal_12s.mp4`
  用途：`S5` 密库真相揭示。
- `videos/cross/x3_final_stance_10s.mp4`
  用途：`X3` 终局前宣言。
- `videos/endings/ending_branch_montage_20s.mp4`
  用途：根据结局分支显示不同镜头序列。

### 视频接线前置条件

在正式生产视频前，建议先补齐以下工程入口：

- 增加资源索引与路径解析（`resources/videos/`）
- 定义播放时机（开场、剧情卡、结局页）
- 提供失败回退路径（缺视频时回退到静态图）
- 补充至少一个视频播放 smoke 测试

## 5. 规格与命名约束

- 文件名统一 ASCII，全部小写，单词用 `_` 分隔。
- 叙事图片统一使用 `story/<segment>/<segment>_<两位序号>_<scene_slug>.png`。
- `main` / `ending` 的 `scene_slug` 优先取剧情卡标题对应的稳定英文短语；`log` 的 `scene_slug` 直接取日志标题 slug。
- 如果是已接线资源改名，执行顺序固定为：`先改文档 -> 再改代码 -> 最后改图片文件名`。
- 视频建议首版采用 `mp4 (H.264, 1080p, 24fps)`，时长控制在 `8s-20s`。
- 资源文档中必须区分：`已接线`、`已规划未接线`、`已接线但缺文件`。

## 6. 执行顺序建议

1. 当前不再把 `P0` 当成待补缺口；若继续做表现层增强，优先推进 `P1`（`W4-W5`、`S4-S5`）的剧情图与关键场景图。
2. 再补 `P2` 的跨区联动和结局反哺图。
3. 视频资产仅入库规划，待代码接线后再转生产优先级。
