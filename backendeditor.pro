DEFINES += BACKENDEDITOR_LIBRARY

Qt += widgets xml

# backendEditor files

SOURCES += \
        befactory.cpp \
        bemode.cpp \
        backendeditorplugin.cpp \
        modewidgets/createwizard.cpp \
        modewidgets/modemainwidget.cpp

HEADERS += \
        befactory.h \
        bemode.h \
        backendeditor_global.h \
        backendeditorconstants.h \
        backendeditoricons.h \
        backendeditorplugin.h \
        modewidgets/createwizard.h \
        modewidgets/modemainwidget.h

DISTFILES += \
        .github/workflows/c-cpp.yml \
        .github/workflows/README.md \
        BackendEditor.json

FORMS += \
    modewidgets/modemainwidget.ui

RESOURCES += \
    backendeditor.qrc \
    template/template.qrc

##########ESSENTIAL MODULE
#base library: load & save to xml or json
include(3rd-party/loadSaveProcessor/src/loadsaveProcessor.pri)

#base Interface:
include(Interface/Interface.pri)

#Data Manager module
include(Tags/Tags.pri)

#Drivers Manager module
include(DriverMgr/DriverMgr.pri)

#Drivers plugins
#include(Drivers/Drivers.pri)

# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "D:/QtGit/work/qt-creator"
message(IDE_SOURCE_TREE: $$IDE_SOURCE_TREE)

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):CONFIG(debug, debug|release) {
    IDE_BUILD_TREE = "D:\QtGit\work\qtc414_qt515_msvc2019_64bit_debug·"
    message("Debug model, IDE_BUILD_TREE: "$$IDE_BUILD_TREE)
}else {
    IDE_BUILD_TREE = "D:\QtGit\work\qtc414_qt515_msvc2019_64bit"
    message("Release model, IDE_BUILD_TREE: "$$IDE_BUILD_TREE)
}

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%QtProjectqtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
#USE_USER_DESTDIR = yes

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
