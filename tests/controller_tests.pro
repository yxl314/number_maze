# Controller + passive view 自测 —— Qt Widgets/QtTest，离屏运行。
QT += core gui widgets testlib
TEMPLATE = app
TARGET   = controller_tests
CONFIG  += console testcase
CONFIG  -= app_bundle

NUMBERMAZE_ROOT = $$PWD/..
include($$NUMBERMAZE_ROOT/number-maze.pri)

HEADERS += \
    $$NUMBERMAZE_CONTROLLER_HEADERS \
    $$NUMBERMAZE_VIEW_HEADERS \
    $$NUMBERMAZE_MODEL_HEADERS \
    $$NUMBERMAZE_SERVICE_HEADERS \
    $$NUMBERMAZE_DOMAIN_HEADERS

SOURCES += \
    $$NUMBERMAZE_CONTROLLER_SOURCES \
    $$NUMBERMAZE_VIEW_SOURCES \
    $$NUMBERMAZE_MODEL_SOURCES \
    $$NUMBERMAZE_SERVICE_SOURCES \
    $$NUMBERMAZE_DOMAIN_SOURCES \
    $$PWD/controller_tests.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
RCC_DIR     = $$OUT_PWD/rcc
