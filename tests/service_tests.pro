# RecordStore 自测 —— 只用 QtCore。
QT += core
QT -= gui
TEMPLATE = app
TARGET   = service_tests
CONFIG  += console
CONFIG  -= app_bundle

NUMBERMAZE_ROOT = $$PWD/..
include($$NUMBERMAZE_ROOT/number-maze.pri)

HEADERS += $$NUMBERMAZE_SERVICE_HEADERS $$NUMBERMAZE_DOMAIN_HEADERS
SOURCES += $$NUMBERMAZE_SERVICE_SOURCES $$NUMBERMAZE_DOMAIN_SOURCES $$PWD/service_tests.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
RCC_DIR     = $$OUT_PWD/rcc
