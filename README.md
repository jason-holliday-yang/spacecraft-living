# SpaceCraft Living

`SpaceCraft Living` 是一个基于 `raylib` 的原生单人生存探索游戏。当前代码库已经收口为混合架构：

- `C` 负责地图、玩家、任务更新等核心运行时
- `C++` 负责资源、UI、会话编排、持久化与内容组织

玩家在外星坠毁后醒来，需要在飞船、森林、孢子沼泽与遗迹之间求生、修复系统，并通过日志与调查任务持续拼合真相。终局不是单纯打穿战斗，而是在收齐关键线索后回到飞船，与 `Loxi` 一起确认自己要走向哪一种结局。

## 当前口径

项目范围已经冻结，后续工作不再扩展新地图、新剧情主线或大型新玩法系统。当前开发重点只有三类：

- 平衡性调整
- 稳定性与兼容性修复
- 维护性整理与发布级收尾

最终玩法基线已经固定为 `Health + Oxygen + Status Bar`。

## 当前能力

当前版本已经具备完整可玩的主循环：

- `Stage 1 - Stage 7` 主线推进
- 区域解锁、地图探索、采集、制作、修复与调查推进
- 以日志和任务解锁为核心的剧情推进链
- `飞船 / Loxi -> 结局分支点 -> 路线执行` 的多结局结构
- `16` 槽手动存档与旧版本兼容加载
- 缺省资源 fallback
- `make` 与 `cmake/ctest` 两套构建验证路径

## 控制

- `WASD` / 方向键：移动
- `F`：交互 / 靠近工作台打开制作
- `Space`：攻击
- `B`：背包
- `N`：洛希面板（当前任务 + 日志归档）
- `M`：地图
- `H`：帮助
- `C`：蹲伏 / 潜行
- `Z`：快速食物
- `X`：快速减压 / 药用资源
- `Esc`：暂停 / 关闭当前界面

## 构建与测试

使用 `Makefile`：

```sh
make
./build/SpaceCraftLiving
```

如果 `raylib` 不在默认路径：

```sh
make RAYLIB_PREFIX=/custom/prefix
```

运行全部 smoke：

```sh
make smoke
```

运行单项 smoke：

```sh
make smoke-session
make smoke-save
make smoke-map
```

使用 `CMake`：

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```

使用 `Xcode`：

```sh
cmake -S . -B build/xcode -G Xcode -DRAYLIB_ROOT=/opt/homebrew
open build/xcode/SpaceCraftLiving.xcodeproj
```

不要直接把仓库根目录或 `build/` 目录作为 Xcode 工程打开。
如果遇到 `CompileDTraceScript` 或 `Multiple commands produce`，参见
[`docs/engineering/Xcode_Setup.md`](docs/engineering/Xcode_Setup.md) 的排障说明。

## 目录入口

- [`docs/README.md`](docs/README.md)：当前有效文档入口
- [`src/README.md`](src/README.md)：源码模块分组与内部边界
- [`include/README.md`](include/README.md)：公共头文件约定
- [`resources/README.md`](resources/README.md)：资源目录说明

推荐阅读顺序：

1. [`docs/design/GAME_DESIGN.md`](docs/design/GAME_DESIGN.md)
2. [`docs/design/CURRENT_STATUS.md`](docs/design/CURRENT_STATUS.md)
3. [`docs/plans/BALANCE_TUNING_PLAN.md`](docs/plans/BALANCE_TUNING_PLAN.md)
4. [`docs/plans/NEXT_STEPS.md`](docs/plans/NEXT_STEPS.md)
5. [`docs/engineering/ARCHITECTURE.md`](docs/engineering/ARCHITECTURE.md)

## 文档使用原则

- `docs/design/` 和 `docs/plans/` 中的活动文档优先于历史文档
- 旧扩展构想、旧阶段总结和旧重构计划只作为背景参考
- 若文档与当前代码冲突，以当前代码和活动文档为准
