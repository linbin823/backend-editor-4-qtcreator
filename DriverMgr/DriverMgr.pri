QT += widgets

INCLUDEPATH += $$PWD\

HEADERS += \
#    $$PWD/SIMDriver/simdriver.h \
#    $$PWD/SIMDriver/simdriverui.h \
    $$PWD/drivermgr.h \
    $$PWD/drivermodel.h \
    $$PWD/driverui.h \
    $$PWD/insertdriver.h

SOURCES += \
#    $$PWD/SIMDriver/simdriver.cpp \
#    $$PWD/SIMDriver/simdriverui.cpp \
    $$PWD/drivermgr.cpp \
    $$PWD/drivermodel.cpp \
    $$PWD/driverui.cpp \
    $$PWD/insertdriver.cpp

FORMS += \
#    $$PWD/SIMDriver/simdriverui.ui \
    $$PWD/driverui.ui \
    $$PWD/insertdriver.ui
