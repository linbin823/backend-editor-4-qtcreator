QT +=  widgets
TEMPLATE      = lib
CONFIG       += plugin
TARGET        = $$qtLibraryTarget(driverMgr)
DESTDIR       = ../../

CONFIG += c++11

#base library: load & save to xml or json
include(../../lib/loadSaveProcessor/src/loadsaveProcessor.pri)

#base Interface:
include(../Interface/Interface.pri)

include(DriverMgr.pri)
