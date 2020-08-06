QT += widgets sql

INCLUDEPATH += $$PWD\

HEADERS += \	
    $$PWD/taginfo.h \
    $$PWD/rtdata.h \
    $$PWD/rtdataui.h \
    $$PWD/rtdatadelegates.h \
    $$PWD/rtdatatablemodel.h

SOURCES += \
    $$PWD/taginfo.cpp \
    $$PWD/rtdata.cpp \
    $$PWD/rtdataui.cpp \
    $$PWD/rtdatatablemodel.cpp

FORMS += \
    $$PWD/rtdataui.ui
