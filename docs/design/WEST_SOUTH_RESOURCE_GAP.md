# SpaceCraft Living 西线 / 南线资源缺口清单（2026-04-11）

## 1. 文档目的

本文档用于回答两个问题：

- 在西线 / 南线补完到 `W5`、`S5` 时，必须新增哪些图片资源。
- 为后续演出增强，哪些视频资源值得准备，以及当前是否已具备播放接线。

配套文档：

- 图片总审计：[`../../resources/images/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/images/README.md)
- 资源目录总览：[`../../resources/README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/README.md)
- 完整剧情稿：[`WEST_SOUTH_FULL_STORY.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_FULL_STORY.md)

## 2. 当前资产状态快照

- 已接线图片：`87` 张路径，`77` 张文件已存在，`10` 张为已接线缺图（地图可读性贴图）。
- 已接线音频：`19` 个 `wav` 全部存在；`7` 个 `bgm_*_1.ogg` 全部存在。
- 视频播放接线：当前代码未检出 `mp4/webm/video` 播放链路。

说明：

- 现阶段“必须补”的第一优先级仍是图片，不是视频。
- 视频资产可先进入规划池，待播放系统接线后再转生产优先级。

## 3. 图片资源新增清单

### P0：已接线但缺图（必须先补）

以下 `10` 张已被运行时代码引用，缺失会退回程序化渲染，但会降低地图可读性：

- `tile_ship_corridor_floor.png`
- `tile_ship_room_floor.png`
- `tile_swamp_outer_ground.png`
- `tile_swamp_deep_ground.png`
- `tile_ruins_path.png`
- `tile_ruins_ring_floor.png`
- `tile_tower_plateau_floor.png`
- `tile_barrier_swamp.png`
- `tile_barrier_deep.png`
- `tile_barrier_ruins.png`

### P1：西线第二轮剧情图（W4-W5）

对应剧情节点：`Echo Basin` 与 `Last Camp`。

建议新增：

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

地图与场景补图建议：

- `tile_echo_basin_floor.png`
- `tile_echo_beacon_array.png`
- `tile_last_camp_tent.png`
- `tile_last_camp_archive_box.png`
- `tile_west_observation_post.png`

### P1：南线第二轮剧情图（S4-S5）

对应剧情节点：`Purifier Ring` 与 `Root Vault`。

建议新增：

- `story/main/main_17_purifier_ring_boot.png`
  对应 `S4` 净化环重启。
- `story/main/main_18_global_risk_drop.png`
  对应 `S4` 风险结构被改写。
- `story/main/main_19_root_vault_core.png`
  对应 `S5` 根系密库核心记录下载。
- `story/logs/log_09_crash_recorder_black_box_residue.png`
  对应《Crash Recorder: Black Box Residue》。
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

地图与场景补图建议：

- `tile_purifier_ring_core.png`
- `tile_purifier_console.png`
- `tile_root_vault_gate.png`
- `tile_root_vault_archive.png`
- `tile_subsurface_cooling_pipe.png`

### P2：跨区联动与终局反哺图（X1-X3）

建议新增：

- `story/main/main_20_trace_correlation.png`
  对应 `X1` 痕迹对照。
- `story/main/main_21_loxi_sync_rewrite.png`
  对应 `X2` 策略重写。
- `story/main/main_22_final_stance.png`
  对应 `X3` 终局前立场确认。
- `story/endings/ending_05_heroic_with_records.png`
- `story/endings/ending_06_peaceful_with_repair.png`
- `story/endings/ending_07_settlement_with_legacy.png`

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

1. 先补完 `P0` 的 10 张缺图，恢复地图可读性。
2. 再补 `P1`（`W4-W5`、`S4-S5`）的剧情图与关键场景图。
3. 最后补 `P2` 的跨区联动和结局反哺图。
4. 视频资产仅入库规划，待代码接线后再转生产优先级。
