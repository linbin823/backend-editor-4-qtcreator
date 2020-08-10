QT += widgets sql

INCLUDEPATH += $$PWD\

HEADERS += \
    $$PWD/tagmgrdelegates.h \
    $$PWD/tagmgrtablemodel.h \
    $$PWD/tagmgrui.h

SOURCES += \
    $$PWD/tag.cpp \
    $$PWD/tagmgr.cpp \
    $$PWD/tagmgrtablemodel.cpp \
    $$PWD/tagmgrui.cpp

FORMS += \
    $$PWD/tagmgrui.ui
