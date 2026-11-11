# GameModel 自测 —— 只用 QtCore，不加载 GUI。
QT += core
QT -= gui
TEMPLATE = app
TARGET   = model_tests
CONFIG  += console
CONFIG  -= app_bundle

NUMBERMAZE_ROOT = $$PWD/..
include($$NUMBERMAZE_ROOT/number-maze.pri)

HEADERS += $$NUMBERMAZE_MODEL_HEADERS $$NUMBERMAZE_DOMAIN_HEADERS
SOURCES += $$NUMBERMAZE_MODEL_SOURCES $$NUMBERMAZE_DOMAIN_SOURCES $$PWD/model_tests.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
RCC_DIR     = $$OUT_PWD/rcc
