# Core 层自测 —— 独立工程
#
#      mkdir -p build/core && cd build/core
#      qmake ../../../../Source/NumberNaze/tests/core_test.pro && make -j && ./core_test
#
# 不依赖 Qt 与 GUI：只编 Core 的两个 .cpp。QT -= 是为了让 qmake 不要去链 Qt 库。
QT -= core gui
TEMPLATE = app
TARGET   = core_test
CONFIG  += console c++17
CONFIG  -= app_bundle
QMAKE_CXXFLAGS += -Wall -Wextra

CORE = $$PWD/..
INCLUDEPATH += $$CORE/Core
HEADERS += $$CORE/Core/GameState.h $$CORE/Core/Graph.h $$CORE/Core/Difficulty.h
SOURCES += $$PWD/core_test.cpp $$CORE/Core/GameState.cpp $$CORE/Core/Graph.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
