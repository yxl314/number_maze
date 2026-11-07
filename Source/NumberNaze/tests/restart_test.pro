# 快照与恢复自测 —— 独立工程
#
#      mkdir -p build/restart && cd build/restart
#      qmake ../../../../Source/NumberNaze/tests/restart_test.pro && make -j && ./restart_test
#
# 不依赖 Qt 与 GUI：只编 Core 的源码。
QT -= core gui
TEMPLATE = app
TARGET   = restart_test
CONFIG  += console c++17
CONFIG  -= app_bundle
QMAKE_CXXFLAGS += -Wall -Wextra

CORE = $$PWD/..
INCLUDEPATH += $$CORE/Core
HEADERS += $$CORE/Core/GameState.h $$CORE/Core/Graph.h $$CORE/Core/Difficulty.h
SOURCES += $$PWD/restart_test.cpp $$CORE/Core/GameState.cpp $$CORE/Core/Graph.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
