# 音频与存档说明

## 本地离线存档

本项目已经实现本地完全离线存档，不依赖网络服务。

默认存档目录：

- `~/Library/Application Support/SpaceCraftLiving/`

其中包含：

- `savegame.bin`
  游戏进度存档
- `settings.bin`
  设置项存档，包括音量与音效开关

## 当前已接入的菜单与存档流程

- 主菜单包含：
  - `开始游戏`
  - `继续游戏`
  - `设置`
- 游戏中按 `ESC` 会弹出暂停窗口：
  - `继续`
  - `保存`
  - `设置`

## 建议补齐的音频文件

放置目录：

- `resources/audio/`

推荐文件：

- `menu_confirm.wav`
  菜单确认、点击按钮
- `menu_open.wav`
  打开暂停菜单、打开设置面板
- `save_done.wav`
  存档成功提示
- `interact_confirm.wav`
  对话、修理、拾取等交互提示
- `ending_success.wav`
  成功结局触发
- `ending_failure.wav`
  失败结局触发
- `menu_loop.ogg`
  主菜单循环背景音乐
- `gameplay_loop.ogg`
  游戏内循环背景音乐
