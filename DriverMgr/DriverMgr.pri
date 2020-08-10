QT += widgets

INCLUDEPATH += $$PWD\

HEADERS += \
#    $$PWD/SIMDriver/simdriver.h \
#    $$PWD/SIMDriver/simdriverui.h \
    $$PWD/driverconfigmodel.h \
    $$PWD/driverconfigui.h \
    $$PWD/insertdriver.h

SOURCES += \
#    $$PWD/SIMDriver/simdriver.cpp \
#    $$PWD/SIMDriver/simdriverui.cpp \
    $$PWD/driverconfigmodel.cpp \
    $$PWD/driverconfigui.cpp \
    $$PWD/drivermgr.cpp \
    $$PWD/insertdriver.cpp

FORMS += \
#    $$PWD/SIMDriver/simdriverui.ui \
    $$PWD/driverconfigui.ui \
    $$PWD/insertdriver.ui
