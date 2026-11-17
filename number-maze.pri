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
# 两个库都采用“平台无关核心 + 内部平台端口”结构：公共源固定，平台适配器按
# qmake 目标平台接入（与源库 CMake 的 WIN32/POSIX 选择保持一致）。singleproc
# 0.3.0 起把 flock/Unix socket 与 LockFileEx/命名管道拆成两个平台文件，避免把
# POSIX 的 poll.h 带进 Windows 构建。
NUMBERMAZE_THIRD_PARTY_HEADERS = \
    $$NUMBERMAZE_ROOT/third_party/singleproc/include/singleproc/singleproc.hpp \
    $$NUMBERMAZE_ROOT/third_party/singleproc/src/platform.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/include/stacklog/stacklog.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/include/stacklog/version.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform_dirs.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform_paths.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform_stream.hpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform_time.hpp

NUMBERMAZE_THIRD_PARTY_SOURCES = \
    $$NUMBERMAZE_ROOT/third_party/singleproc/src/singleproc.cpp \
    $$NUMBERMAZE_ROOT/third_party/stacklog/src/stacklog.cpp

unix {
    NUMBERMAZE_THIRD_PARTY_SOURCES += \
        $$NUMBERMAZE_ROOT/third_party/singleproc/src/platform_posix.cpp \
        $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform_posix.cpp
}

win32 {
    NUMBERMAZE_THIRD_PARTY_SOURCES += \
        $$NUMBERMAZE_ROOT/third_party/singleproc/src/platform_win32.cpp \
        $$NUMBERMAZE_ROOT/third_party/stacklog/src/platform_win32.cpp
    # singleproc 的 Windows 门铃用命名事件/安全描述符，需要 advapi32。
    LIBS += -ladvapi32
}

INCLUDEPATH += \
    $$NUMBERMAZE_ROOT/src \
    $$NUMBERMAZE_ROOT/third_party/singleproc/include \
    $$NUMBERMAZE_ROOT/third_party/stacklog/include

# Qt 5.5 的 qmake 不支持 CONFIG += c++14，直接指定标准。
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -Wall -Wextra
