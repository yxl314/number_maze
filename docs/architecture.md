# 架构：分层 MVC + 被动视图

本项目采用与 `life_game` 相同的分层约束。验收标准是目录、include 方向、
`number-maze.pri` 模块清单和测试行为同时成立，而不是文档自述。

## 1. 依赖规则

```text
                         app
                          │
                          ▼
                      controller
                    ╱     │      ╲
                   ▼      ▼       ▼
                 view   model   service
                   ╲      │      ╱
                    ╲     ▼     ╱
                      domain
```

| 层 | 目录 | 允许依赖 | 职责 |
|---|---|---|---|
| Domain | `src/domain` | 自身、C++ 标准库 | 图拓扑、局面、移动/撤销/打乱、难度档位 |
| Model | `src/model` | Domain、QtCore | 可观察游戏状态、计时、内存内最佳成绩 |
| Service | `src/service` | Domain、QtCore | QSettings 最佳成绩持久化 |
| View | `src/view` | Domain、QtWidgets | 被动棋盘、状态栏、难度文案、对话框 |
| Controller | `src/controller` | Domain、Model、Service、View | 唯一 Model↔View 中介、记录读写协调 |
| App | `src/app` | 全部 | 入口和组合根 |

**禁止方向**：

- View 不 include `model/`、`service/`、`controller/`、`app/`；
- Model 不 include `service/`、`view/`、`controller/`、`app/`，不碰 QSettings；
- Service 不 include Model/View/Controller/App；
- Domain 不包含任何 Qt 头文件或中文字面量；
- Controller 不直接构造具体对话框或操作具体 QLabel/QAction。

`tools/check_layering.py` 会检查 include 方向、Qt/中文污染、清单完整性，
以及 `NUMBERMAZE_*_SOURCES/HEADERS` 分组是否与文件所在层一致。

## 2. 各层职责

### Domain

- `domain::Graph`：固定的 22 边图拓扑，不持有局面。
- `domain::GameState`：16 格棋盘、合法移动、撤销栈、打乱、快照恢复。
- `domain::Difficulty`：难度枚举和打乱步数，不包含显示文案。
- 纯 C++14，可脱离 Qt 单独编译测试。

### Model

- `model::GameModel`：包装 `domain::GameState`，提供板面、可移动性、状态信号。
- 管理计时器和内存内最佳成绩；不读写文件，不显示文案，不持有 View。
- 状态变化通过 `boardChanged`、`statusChanged`、`solved` 广播给 Controller。

### Service

- `service::RecordStore`：唯一接触 QSettings 的类。
- 通过 `bestMoves()/saveBestMoves()` 持久化各难度的最佳步数。
- 不依赖 Model，因此可以单独测试。

### View

- `view::BoardView`：被动棋盘；只接收 `BoardFrame`、发出 `nodeClicked`。
  不 include Model，不主动查询任何状态。
- `view::DifficultyCatalog`：Domain 难度枚举到中文名称的展示映射。
- `view::StatusBarView`：实现 `StatusSink`，更新四个标签、窗口标题和悔棋动作。
- `view::MessageBoxSolvedSink`：实现 `SolvedSink`，负责通关弹窗。
- `view::showAbout()`：关于对话框。
- `view::BoardFrame`/`StatusState`/`SolvedInfo`：跨 Controller→View 的值对象。

### Controller

- `controller::GameController`：
  - 把 `BoardView::nodeClicked` 转成 `GameModel::requestMove`；
  - 订阅 Model 信号，构造 `BoardFrame`/`StatusState` 并推给被动 View；
  - 初始化时从 `RecordStore` 读取最佳成绩，通关时写回；
  - 通过 `StatusSink`/`SolvedSink` 端口更新外壳，不持有 QLabel/QAction。
- 不包含游戏规则，不包含绘制代码，不 include `app/`。

### App

- `app::main`：QApplication、字体/主题、日志、单实例守卫、主窗口。
- `app::MainWindow`：组合 Model/View/Service/Controller；连接菜单和工具栏。
- 不直接调用 Model 命令；首局由 Controller 的 `initialize()` 启动。
- 只做组合与菜单转发，不做信号中介。

## 3. 数据流

### 用户移动

```text
BoardView::nodeClicked(index)
  → GameController::handleNodeClicked
  → GameModel::requestMove
  → domain::GameState::move
  → GameModel::boardChanged / statusChanged
  → GameController::syncBoardToView / refreshStatus
  → BoardView::applyFrame(BoardFrame)
  → StatusBarView::applyStatus(StatusState)
```

### 开局/重开/悔棋

```text
菜单/工具栏 → GameController slot
  → GameModel::startNewGame / restartCurrentGame / requestUndo
  → 同一条 Model→Controller→View 推送路径
```

### 最佳成绩

```text
Controller::initialize
  → RecordStore::bestMoves(每个难度)
  → GameModel::setBestRecords
  → StatusBarView 显示“最佳”

GameModel 判定新纪录 → solved(moves, seconds, true)
  → Controller::handleSolved
  → RecordStore::saveBestMoves
  → MessageBoxSolvedSink
```

## 4. 构建与模块清单

- `number-maze.pri` 是唯一源文件清单，按 `NUMBERMAZE_DOMAIN_*`、
  `NUMBERMAZE_MODEL_*`、`NUMBERMAZE_SERVICE_*`、`NUMBERMAZE_VIEW_*`、
  `NUMBERMAZE_CONTROLLER_*`、`NUMBERMAZE_APP_*`、`NUMBERMAZE_MAIN_SOURCES`
  分组。
- 主程序全量加入；测试按覆盖范围加入子集。新增文件必须显式加入清单。
- 第三方源码 `third_party/singleproc`、`third_party/stacklog` 只由主程序加入，
  测试不依赖它们。
- 并行上限固定为 6：`tests/run_all.sh` 的 `JOBS=6`、`ci.toml` 的 `-j6`、
  `audit.toml` 的 `jobs=6`、`cxx_pkg.ini` 的 `make_args=-j6`。

## 5. 错误与持久化

- Domain 的 `restore()` 会验证快照是 0..15 的完整排列；非法快照返回 false
  且不改变状态。
- `Graph`/`GameState` 对越界索引返回空邻居、0 或 -1，不越界访问。
- RecordStore 读不到记录返回 0；没有 QSettings 实例时也不崩溃。
- Model 不依赖文件系统；测试可只构造 `GameModel` 验证状态机。

## 6. 测试与门禁

| 工程 | 依赖 | 覆盖 |
|---|---|---|
| `domain_tests` | 无 Qt | 图拓扑、移动/撤销、打乱、可解性 |
| `restart_tests` | 无 Qt | 快照恢复及非法快照拒绝 |
| `model_tests` | QtCore | 重开本局、难度切换、状态 |
| `service_tests` | QtCore | 最佳成绩持久化 |
| `controller_tests` | QtWidgets/QtTest | 被动 View 推送、状态栏、悔棋、重开、记录加载 |

门禁：

```bash
python3 tools/check_layering.py .
python3 tools/layering_selftest.py .
```

`layering_selftest` 用负向 fixture 验证 View→Model、Domain→Qt、清单分组错位
等绕过确实会失败。
