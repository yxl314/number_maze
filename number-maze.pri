# ---------------------------------------------------------------------------
# number-maze module manifest
#
# 每个 .pro 只加入自己需要的模块组。主程序与测试都 include 本文件，因此
# 依赖方向在构建清单上可见；不要用 glob 自动收集源文件。
# ---------------------------------------------------------------------------

NUMBERMAZE_ROOT = $$PWD

NUMBERMAZE_DOMAIN_HEADERS = \
    $$NUMBERMAZE_ROOT/src/domain/difficulty.h \
    $$NUMBERMAZE_ROOT/src/domain/graph.h \
    $$NUMBERMAZE_ROOT/src/domain/game_state.h

NUMBERMAZE_DOMAIN_SOURCES = \
    $$NUMBERMAZE_ROOT/src/domain/graph.cpp \
    $$NUMBERMAZE_ROOT/src/domain/game_state.cpp

NUMBERMAZE_MODEL_HEADERS = \
    $$NUMBERMAZE_ROOT/src/model/game_model.h

NUMBERMAZE_MODEL_SOURCES = \
    $$NUMBERMAZE_ROOT/src/model/game_model.cpp

NUMBERMAZE_SERVICE_HEADERS = \
    $$NUMBERMAZE_ROOT/src/service/record_store.h

NUMBERMAZE_SERVICE_SOURCES = \
    $$NUMBERMAZE_ROOT/src/service/record_store.cpp

NUMBERMAZE_VIEW_HEADERS = \
    $$NUMBERMAZE_ROOT/src/view/board_view.h \
    $$NUMBERMAZE_ROOT/src/view/difficulty_catalog.h \
    $$NUMBERMAZE_ROOT/src/view/interfaces.h \
    $$NUMBERMAZE_ROOT/src/view/status_bar_view.h \
    $$NUMBERMAZE_ROOT/src/view/dialogs.h \
    $$NUMBERMAZE_ROOT/src/view/view_state.h

NUMBERMAZE_VIEW_SOURCES = \
    $$NUMBERMAZE_ROOT/src/view/board_view.cpp \
    $$NUMBERMAZE_ROOT/src/view/difficulty_catalog.cpp \
    $$NUMBERMAZE_ROOT/src/view/status_bar_view.cpp \
    $$NUMBERMAZE_ROOT/src/view/dialogs.cpp

NUMBERMAZE_CONTROLLER_HEADERS = \
    $$NUMBERMAZE_ROOT/src/controller/game_controller.h

NUMBERMAZE_CONTROLLER_SOURCES = \
    $$NUMBERMAZE_ROOT/src/controller/game_controller.cpp

NUMBERMAZE_APP_HEADERS = \
    $$NUMBERMAZE_ROOT/src/app/main_window.h

NUMBERMAZE_APP_SOURCES = \
    $$NUMBERMAZE_ROOT/src/app/main_window.cpp

NUMBERMAZE_MAIN_SOURCES = \
    $$NUMBERMAZE_ROOT/src/app/main.cpp

# 第三方源码保持独立；域层和测试不依赖它们。
NUMBERMAZE_THIRD_PARTY_HEADERS = \
    $$NUMBERMAZE_ROOT/third_party/singleproc/include/singleproc/singleproc.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/include/stacklog/stacklog.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/include/stacklog/version.hpp

NUMBERMAZE_THIRD_PARTY_SOURCES = \
    $$NUMBERMAZE_ROOT/third_party/singleproc/src/singleproc.cpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/stacklog.cpp

INCLUDEPATH += \
    $$NUMBERMAZE_ROOT/src \
    $$NUMBERMAZE_ROOT/third_party/singleproc/include \
    $$NUMBERMAZE_ROOT/third_party/stacklog/include

# Qt 5.5 的 qmake 不支持 CONFIG += c++14，直接指定标准。
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -Wall -Wextra
