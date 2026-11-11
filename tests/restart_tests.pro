# 快照/恢复自测 —— 独立工程，不依赖 Qt/GUI。
QT -= core gui
TEMPLATE = app
TARGET   = restart_tests
CONFIG  += console
CONFIG  -= app_bundle

NUMBERMAZE_ROOT = $$PWD/..
include($$NUMBERMAZE_ROOT/number-maze.pri)

HEADERS += $$NUMBERMAZE_DOMAIN_HEADERS
SOURCES += $$NUMBERMAZE_DOMAIN_SOURCES $$PWD/restart_tests.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
RCC_DIR     = $$OUT_PWD/rcc
