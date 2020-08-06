QT+= widgets

TEMPLATE      = lib
CONFIG       += plugin
TARGET        = $$qtLibraryTarget(SimDriver)
DESTDIR       = ../../Drivers

CONFIG += c++11

INCLUDEPATH += $$PWD\

HEADERS += \
    $$PWD/simdriver.h \
    $$PWD/simdriverui.h
SOURCES += \
    $$PWD/simdriver.cpp \
    $$PWD/simdriverui.cpp
FORMS += \
    $$PWD/simdriverui.ui

#interface:
include(../../Interface/Interface.pri)

#base library: load & save to xml or json
include(../../3rd-party/loadSaveProcessor/src/loadsaveProcessor.pri)
