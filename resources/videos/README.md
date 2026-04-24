# SpaceCraft Living 视频资源说明

## 当前状态

审计日期：`2026-04-20`

- 当前代码未发现视频播放接线。
- `resources/videos/` 目录目前作为规划目录使用。
- 本目录内文件不计入“当前已接线资源”统计。
- 当前建议先按“短演出视频”规划，而不是全面替代静态剧情图。
- 第一批“可能加入”的视频资源位，建议先落在 `5` 段主线关键节点。

## 推荐目录结构（规划）

- `resources/videos/intro/`
  开场回顾和过场视频。
- `resources/videos/main/`
  主线剧情短演出，按 `M01-M30` 槽位规划。
- `resources/videos/south/`
  南线关键演出。
- `resources/videos/north/`
  北线 / 塔区演出。
- `resources/videos/endings/`
  结局分支演出。

## 命名与规格建议

- 主线短演出命名：`story_main_m##_<scene_slug>_v001.mp4`
- 结局短演出命名：`story_ending_e##_<_scene_slug_>_v001.mp4`
- 比例：`16:10`
- 分辨率：优先 `1312x816`，如需高分版可扩到 `1920x1200`
- 帧率：`24fps`
- 编码：`H.264 + AAC`
- 单段时长：`4s-6s`
- 设计原则：先播放短视频，再落回对应静态剧情图；静图仍作为阅读层和缺视频回退层保留。

## 第一批可能加入的视频资源位

以下条目当前仅为资源规划，不代表代码已经接线：

1. `resources/videos/main/story_main_m04_airlock_opening_v001.mp4`
   - 对应：`M04 气闸开启`
   - 建议时长：`5s`
   - 演出重点：气闸门缓慢开启、冷雾外泄、外部世界显露

2. `resources/videos/south/story_main_m17_vent_network_calibrated_v001.mp4`
   - 对应：`M17 通风网络校准`
   - 建议时长：`5s`
   - 演出重点：风阀启动、管线亮起、毒雾缓慢退散

3. `resources/videos/south/story_main_m19_purifier_ring_sequence_v001.mp4`
   - 对应：`M19 净化环恢复次序`
   - 建议时长：`6s`
   - 演出重点：净化环按顺序分段点亮，环境压力回落

4. `resources/videos/north/story_main_m24_north_route_commitment_v001.mp4`
   - 对应：`M24 北线路线承诺`
   - 建议时长：`5s`
   - 演出重点：宇航员朝高塔缓慢前行，雾中塔体压近

5. `resources/videos/endings/story_main_m29_peaceful_route_commitment_v001.mp4`
   - 对应：`M29 和平路线前章`
   - 建议时长：`5s`
   - 演出重点：稳定接入、光束升起、系统平稳回应

## AI 生成口径建议

- 统一风格：
  `retro pixel art cinematic cutscene, detailed sci-fi survival, atmospheric fog, dramatic lighting, teal-blue palette with subtle orange accents`
- 统一人物：
  `same astronaut as existing game art, cream-white bulky EVA suit, orange stripes, dark blue-gray visor, tan rectangular backpack with cyan lights`
- 统一镜头：
  人物尽量保持中小比例，优先做环境运动，不做大幅面部或肢体表演。
- 统一用途：
  视频负责演出冲击，静态剧情图负责停留阅读；两者并存，不互相替代。

## 接线前置条件

在没有播放系统前，不建议批量生产视频。建议先完成：

- 视频资源路径索引
- UI / 剧情页播放触发点
- 缺视频回退到静态图
- 基础视频播放 smoke 测试

## 关联文档

- [`../../docs/design/WEST_SOUTH_RESOURCE_GAP.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_RESOURCE_GAP.md)
- [`../README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/README.md)
