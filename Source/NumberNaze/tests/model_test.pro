# GameModel 自测 —— 独立工程
#
#      mkdir -p build/model && cd build/model
#      qmake ../../../../Source/NumberNaze/tests/model_test.pro && make -j && ./model_test
#
# 不依赖 GUI（只用 QCoreApplication），所以 QT -= gui。
QT += core
QT -= gui
TEMPLATE = app
TARGET   = model_test
CONFIG  += console c++17
CONFIG  -= app_bundle
QMAKE_CXXFLAGS += -Wall -Wextra

CORE = $$PWD/..
INCLUDEPATH += $$CORE/Core $$CORE/MVC/Model
HEADERS += $$CORE/MVC/Model/GameModel.h \
           $$CORE/Core/GameState.h $$CORE/Core/Graph.h $$CORE/Core/Difficulty.h
SOURCES += $$PWD/model_test.cpp \
           $$CORE/MVC/Model/GameModel.cpp \
           $$CORE/Core/GameState.cpp $$CORE/Core/Graph.cpp

DESTDIR     = $$OUT_PWD/bin
OBJECTS_DIR = $$OUT_PWD/obj
MOC_DIR     = $$OUT_PWD/moc
