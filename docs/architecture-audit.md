# 分层 MVC 复审

复审基准：分层 MVC + 被动视图。证据来自源码 include、`number-maze.pri`、
测试依赖和实际运行结果，不以架构文档自证。

## 审核结果

| 规则 | 证据 | 结果 |
|---|---|---|
| View 不认识 Model | `grep -rn '#include "model/' src/view` 无输出 | 通过 |
| View 不持有 Model | View 只 include `view/`、`domain/` 和 QtWidgets | 通过 |
| View 是原子被动推送 | `BoardView::applyFrame(BoardFrame)`；`StatusBarView::applyStatus(StatusState)` | 通过 |
| Controller 是唯一 Model↔View 中介 | `GameController` 订阅 Model 信号，调用 View 端口 | 通过 |
| Controller 不操作具体 QLabel/QAction | Controller 只依赖 `StatusSink`/`SolvedSink` | 通过 |
| Service 不依赖 Model/View | `RecordStore` 只 include Domain/QtCore | 通过 |
| Model 不依赖 Service/View/Controller | `GameModel` 不 include 它们，不接触 QSettings | 通过 |
| Domain 无 Qt | Domain 无 `<Q...>` include，无中文展示文案 | 通过 |
| App 不直接命令 Model | `MainWindow` 只组合并连接 Controller；首局由 Controller 启动 | 通过 |
| 模块清单与依赖一致 | `number-maze.pri` 分组 + `check_layering.py` | 通过 |
| 非法输入/越界安全 | `Graph`/`GameState` 越界返回安全值；`restore()` 校验 0..15 排列 | 通过 |

## 当前限制

- 兼容目标仍是 Qt 5.5 / glibc 2.23 的构建环境；本机使用 Qt 5.15 验证。
  Qt 6 可编译性不作为验收条件。
- 最佳成绩持久化没有版本迁移机制；键名固定为 `records/<difficulty>`。
- `GameState::restore()` 验证排列合法性，但不验证任意排列在该图上的可解性；
  当前快照只来自本局打乱结果和合法的 `board()`，因此运行时路径安全。
- `singleproc`、`stacklog` 是嵌入式第三方源码，不纳入本项目的分层检查；
  主程序是唯一链接它们的工程。

## 复核命令

```bash
python3 ../cxx_ci/ci/ci.py -c ci.toml
python3 ../cxx_pkg/cxx_pkg.py --config cxx_pkg.ini --format tar.gz
PYTHONPATH=../cxx_audit/src python3 -m cpp_audit -c audit.toml all
python3 tools/check_layering.py .
python3 tools/layering_selftest.py .
```

当前结果：5 个测试工程通过；分层门禁 24 个源文件通过；CPD 70 token 无重复；
Valgrind 0 definite/indirect leak；cxx_pkg 的 tar.gz 与 .run 包均可安装并 offscreen 启动。
