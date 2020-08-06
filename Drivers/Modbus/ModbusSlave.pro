QT           += serialport widgets

TEMPLATE      = lib
CONFIG       += plugin
TARGET        = $$qtLibraryTarget(modbusSlave)
DESTDIR       = ../../Drivers

INCLUDEPATH += $$PWD\

CONFIG+= C++11

HEADERS += \
    $$PWD/modbusslaveprotocoldriver.h \
#    $$PWD/modbusmasterprotocoldriverui.h \
#    $$PWD/modbusmasterparametersmodel.h \
#    $$PWD/modbusmasterparametersdelegate.h \
    $$PWD/modbusdataunit.h \
    $$PWD/libmodbus/modbus.h \
    $$PWD/libmodbus/modbus-private.h \
    $$PWD/libmodbus/modbus-rtu.h \
    $$PWD/libmodbus/modbus-rtu-private.h \
    $$PWD/libmodbus/modbus-tcp.h \
    $$PWD/libmodbus/modbus-tcp-private.h \
    $$PWD/libmodbus/modbus-version.h \
    $$PWD/libmodbus/win32/config.h \
    $$PWD/libmodbus/linux/config.h

SOURCES += \
    $$PWD/modbusslaveprotocoldriver.cpp \
#    $$PWD/modbusmasterprotocoldriverui.cpp \
#    $$PWD/modbusmasterparametersmodel.cpp \
    $$PWD/libmodbus/modbus.c \
    $$PWD/libmodbus/modbus-data.c \
    $$PWD/libmodbus/modbus-rtu.c \
    $$PWD/libmodbus/modbus-tcp.c

#FORMS += \
#    $$PWD/modbusmasterprotocoldriverui.ui


win32:LIBS += -lWS2_32

#interface:
include(../../Interface/Interface.pri)

#base library: load & save to xml or json
include(../../../lib/loadSaveProcessor/src/loadsaveProcessor.pri)
