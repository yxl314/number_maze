# 数字迷宫 (number-maze) —— Qt 5
#
# 目标：保持 Ubuntu 16.04（Qt 5.5.1 / g++ 5.4）构建兼容性，同时采用与
# life_game 相同的分层目录、显式模块清单和检查门禁。

QT += core gui widgets

TARGET   = number-maze
TEMPLATE = app

# 默认 release；Qt Creator / cpp-audit 的 Debug 配置会传入 CONFIG+=debug。
# 不能无条件 CONFIG += release，否则 Debug 也会带 -O2 和 QT_NO_DEBUG，
# 既没有优化关闭，也会丢掉调试符号。
CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS_DEBUG += -O0 -g
} else {
    CONFIG += release
    QMAKE_CXXFLAGS_RELEASE += -O2
}

CONFIG -= app_bundle
CONFIG -= qtquickcompiler

DEFINES += QT_DEPRECATED_WARNINGS

include($$PWD/number-maze.pri)

HEADERS += \
    $$NUMBERMAZE_DOMAIN_HEADERS \
    $$NUMBERMAZE_MODEL_HEADERS \
    $$NUMBERMAZE_SERVICE_HEADERS \
    $$NUMBERMAZE_VIEW_HEADERS \
    $$NUMBERMAZE_CONTROLLER_HEADERS \
    $$NUMBERMAZE_APP_HEADERS \
    $$NUMBERMAZE_THIRD_PARTY_HEADERS

SOURCES += \
    $$NUMBERMAZE_DOMAIN_SOURCES \
    $$NUMBERMAZE_MODEL_SOURCES \
    $$NUMBERMAZE_SERVICE_SOURCES \
    $$NUMBERMAZE_VIEW_SOURCES \
    $$NUMBERMAZE_CONTROLLER_SOURCES \
    $$NUMBERMAZE_APP_SOURCES \
    $$NUMBERMAZE_MAIN_SOURCES \
    $$NUMBERMAZE_THIRD_PARTY_SOURCES

RESOURCES += resources/resources.qrc

# cxx_pkg / make install 的安装规则：配合 INSTALL_ROOT 落到 staging。
target.path = /opt/number-maze
INSTALLS += target

# stacklog 使用 std::thread，需显式链接 pthread。
unix: LIBS += -pthread

# 产物落在构建目录中，源码目录保持干净。
DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
RCC_DIR     = $$OUT_PWD/rcc
