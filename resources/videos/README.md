# SpaceCraft Living 视频资源说明

## 当前状态

审计日期：`2026-04-11`

- 当前代码未发现视频播放接线。
- `resources/videos/` 目录目前作为规划目录使用。
- 本目录内文件不计入“当前已接线资源”统计。

## 推荐目录结构（规划）

- `videos/intro/`
  开场回顾和过场视频。
- `videos/west/`
  西线 `W4-W5` 关键演出。
- `videos/south/`
  南线 `S4-S5` 关键演出。
- `videos/cross/`
  跨区联动 `X1-X3` 演出。
- `videos/endings/`
  结局分支演出。

## 命名与规格建议

- 命名：`<segment>_<event>_<duration>.mp4`
- 分辨率：`1920x1080`
- 帧率：`24fps`
- 编码：`H.264 + AAC`
- 时长：`8s-20s`

## 接线前置条件

在没有播放系统前，不建议批量生产视频。建议先完成：

- 视频资源路径索引
- UI / 剧情页播放触发点
- 缺视频回退到静态图
- 基础视频播放 smoke 测试

## 关联文档

- [`../../docs/design/WEST_SOUTH_RESOURCE_GAP.md`](/Users/jason/Documents/SpaceCraftLivng/docs/design/WEST_SOUTH_RESOURCE_GAP.md)
- [`../README.md`](/Users/jason/Documents/SpaceCraftLivng/resources/README.md)

