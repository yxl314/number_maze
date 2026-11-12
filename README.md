# 数字迷宫 (number-maze) —— Qt 5 版

一款基于**特殊图连接拓扑**的数字滑块游戏。把 1 到 15 的数字重新排回顺序、
空格归位右下角即可通关。

项目使用 **Qt 5 + C++14**，保持 Qt 5.5 构建兼容性，同时采用与 `life_game`
相同的分层 MVC、显式模块清单、cxx_ci/cxx_pkg 管理和检查门禁。
发行包由 cxx_pkg 收集 Qt 运行库，目标是在 **Ubuntu 16.04 ~ 26.04** 上运行。

## 与普通数字华容道的区别

普通华容道使用 4×4 规则网格，只能上下左右推动，玩家凭空间直觉就能判断
“空格周围的格子可以动”。

数字迷宫的节点虽然也按 4×4 方阵摆放，但连接关系是专门设计的拓扑：共
22 条边，其中包含多条**对角线连接**（例如 1–6、3–8）。因此不能靠位置
直觉判断可移动性，必须观察实际连线。全图度数分布也不均匀（4 个节点度为
2，12 个节点度为 3），进一步提高了难度。

界面**强制显示连线与可移动高亮**，不提供关闭选项——这两种视觉辅助是
玩法必需的信息。

## 玩法

- 点击与空格相连的数字，它会滑入空格
- 绿色高亮的节点表示当前可移动，虚线圆圈表示空格
- 把所有数字恢复成 `1..15` 顺序（空格在右下角）即通关

| 操作 | 方式 |
|---|---|
| 移动数字 | 鼠标点击 |
| 悔棋 | 工具栏按钮 / `Ctrl+Z` |
| 重开本局 | 工具栏按钮 / `Ctrl+R` |
| 更换难度 | 菜单「游戏」→「新游戏」 |

| 难度 | 打乱步数 |
|---|---|
| 简单 | 20 |
| 普通 | 40 |
| 困难 | 80 |
| 专家 | 160 |

打乱是从已解状态出发执行若干次**合法移动**，因此任何开局都必然可解；
打乱不计入撤销历史，避免用悔棋一路退回已解状态。

## 架构

采用分层 MVC + 被动视图，依赖方向单向向下，View 与 Model 互不相识：

```
app::MainWindow（组合根）
   ├── view::BoardView / StatusBarView / DifficultyCatalog / 对话框
   ├── model::GameModel（状态、计时、内存内最佳成绩）
   ├── service::RecordStore（QSettings 持久化）
   └── controller::GameController（唯一 Model↔View 中介）

domain::Graph / GameState / Difficulty（纯 C++14，无 Qt）
```

- **Domain**：图拓扑、移动/撤销/打乱、快照校验；无 Qt、无中文展示文案。
- **Model**：只依赖 Domain/QtCore，不读写文件、不持有 View。
- **Service**：唯一接触 QSettings 的层，负责最佳成绩。
- **View**：`BoardView::applyFrame(BoardFrame)` 原子接收棋盘；`StatusBarView`
  实现 `StatusSink`；只发 `nodeClicked` 意图，不查询 Model。
- **Controller**：把 View 意图转成 Model 命令，把 Model 状态构造成
  `BoardFrame`/`StatusState` 推给 View；通过端口显示状态和通关弹窗。
- **App**：只组合对象和连接菜单；首局由 Controller 的 `initialize()` 启动。

数据流：用户点击 → `BoardView::nodeClicked` → `GameController` →
`GameModel::requestMove` → `domain::GameState` → Model 信号 →
Controller 构造帧 → View 重绘。

详细设计见 [docs/architecture.md](docs/architecture.md)，复审记录见
[docs/architecture-audit.md](docs/architecture-audit.md)。

## 构建

需要 Qt 5（Widgets）、qmake、支持 C++14 的编译器。Ubuntu 16.04 上：

```bash
sudo apt-get install -y qtbase5-dev qt5-qmake g++ make
mkdir -p build/app && cd build/app
qmake ../../number-maze.pro && make -j4
./bin/number-maze
```

本机 qmake 可能被 Qt 6 占用；需要显式使用 Qt 5 时：

```bash
qmake5 ../../number-maze.pro && make -j4
```

Qt 5.5 的 qmake 不支持 `CONFIG += c++14`，`number-maze.pri` 直接指定
`-std=c++14`，因此 g++ 5.4 也能编译。产物落在构建目录的 `bin/`，源码目录保持干净。

### cxx_ci

```bash
python3 ../cxx_ci/ci/ci.py -c ci.toml
```

流水线执行 clean → configure → build → 5 个测试工程，固定 6 路并行。

## 测试

五个测试各自是独立 qmake 工程，不依赖主程序，也不依赖预先构建的库：

```bash
QMAKE=qmake5 ./tests/run_all.sh
QMAKE=qmake5 ./tests/run_all.sh model   # 只跑名字含 model 的
```

| 工程 | 依赖 | 覆盖 |
|---|---|---|
| `domain_tests` | 无 Qt | 图结构、移动/撤销、打乱、可解性 |
| `restart_tests` | 无 Qt | 局面快照恢复、非法快照拒绝 |
| `model_tests` | QtCore | 重开本局、难度切换、状态 |
| `service_tests` | QtCore | 最佳成绩持久化 |
| `controller_tests` | QtWidgets/QtTest | Model→View 推送、状态栏、悔棋、重开、记录加载 |

产物落在 `build/tests/<name>/bin/`，不污染源码目录。`run_all.sh` 默认 6 路并行，
测试界面时导出 `QT_QPA_PLATFORM=offscreen`，无需 Xvfb。

### 架构门禁

```bash
python3 tools/check_layering.py .
python3 tools/layering_selftest.py .
```

检查 include 方向、Qt/中文污染、`number-maze.pri` 清单完整性和模块分组错位；
selftest 用负向 fixture 证明门禁会失败。

### 检查入口

```bash
./check.sh all     # CI + standardize + audit
./check.sh ci      # 构建与测试
./check.sh style   # 主工程 src 的全量 tidy
./check.sh audit   # clang-tidy + CPD + Valgrind
```

`standardize` 覆盖主工程 `src`；测试是独立 qmake 工程，不在主工程的
`compile_commands.json` 中，由 `cxx_ci` 负责构建和运行。禁改区/第三方代码由
`exclude` 排除，Qt 自身头文件由 `.clang-tidy` 的 `HeaderFilterRegex` 忽略。

## 打包（cxx_pkg）

打包配置在 `cxx_pkg.ini`，由 cxx_pkg 完成构建、运行时依赖收集、Qt 插件部署、
RPATH 改写和包输出：

```bash
python3 ../cxx_pkg/cxx_pkg.py --config cxx_pkg.ini
# 产物：dist/number-maze-2.0.0-aarch64.tar.gz
#       dist/number-maze-2.0.0-aarch64.run
```

本地 Qt 5 的路径由配置显式指定：

```ini
[build]
qmake = qmake5
make_args = -j4

[qt]
enabled = true
prefix = /usr
```

包内会自动带上：

- Qt5 Core / Gui / Widgets / DBus / XcbQpa 等实际依赖
- `libqxcb.so`、`libqoffscreen.so`、`libqminimal.so` 平台插件
- 依赖的 X11/GL/fontconfig 等运行库（由 `ldd` 递归收集）
- 可执行文件旁的 `qt.conf` 和相对 RPATH

非 root 验证 `.run`：

```bash
DESTDIR=/tmp/number-maze-root ./dist/number-maze-2.0.0-aarch64.run
/tmp/number-maze-root/opt/number-maze/number-maze
```

**兼容性基线**：包内的 glibc 基线由构建机决定。要在 Ubuntu 16.04 上运行，
应在 16.04（Qt 5.5.1）环境执行同一份 `cxx_pkg.ini` 完成构建与打包；
在较新 Ubuntu 上构建的二进制不能保证回退到 16.04。

`.run` 是 cxx_pkg 生成的自解压脚本，支持 `DESTDIR` 安装；
卸载时删除安装前缀下的 `number-maze` 目录即可。

## Qt 5 适配要点

- `<QAction>` 在 Qt 5 属于 QtWidgets；系统若同时装了 Qt3D，可能有同名头
  文件抢先命中，因此显式写 `#include <QtWidgets/QAction>`。
- Qt 5.5 没有 `QFontMetrics::horizontalAdvance`，用 `QT_VERSION` 分支退回
  `width()`。
- 固定使用 Fusion 风格，不依赖各发行版的 gtk 主题插件。
- 内置一份中文字体子集，目标机缺少 CJK 字体时也不会出现方框。
- “重开本局”使用本局开局快照复位，并由 `tests/model_tests.cpp` 覆盖。

## 单实例与日志

`main.cpp` 使用嵌入式 singleproc 库保证同一用户只运行一个实例；当前
应用暂未接入门铃，重复启动会直接退出。日志由
stacklog 写入 `$XDG_STATE_HOME/number-maze/number-maze.log`
（未设该变量时为 `~/.local/state/number-maze/number-maze.log`），
文件按 2 MiB × 3 份轮转。

## 目录

```
number-maze.pro             qmake 主工程
number-maze.pri             分层模块源文件清单
src/
  domain/                   纯 C++14：Graph / GameState / Difficulty
  model/                    GameModel：状态、计时、内存内成绩
  service/                  RecordStore：QSettings 持久化
  view/                     BoardView / StatusBarView / 文案与对话框
  controller/               GameController：唯一 Model↔View 中介
  app/                      main + MainWindow 组合根
tests/                      5 个独立 qmake 测试工程与 run_all.sh
tools/                      check_layering.py + 负向 selftest
docs/                       architecture.md / architecture-audit.md
third_party/singleproc/     内嵌库：单实例守卫
third_party/stacklog/       内嵌库：调用链日志
resources/                  内置字体子集、图标与 qrc
audit.toml                  cxx_audit 配置
ci.toml                     cxx_ci 配置
cxx_pkg.ini                 cxx_pkg 打包配置
```
