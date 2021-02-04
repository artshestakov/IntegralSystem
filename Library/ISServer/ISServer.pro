QT += core sql
QT -= gui

TARGET = ISServer
TEMPLATE = lib
CONFIG += c++17
DEFINES += ISSERVER_LIBRARY

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
    DEFINES += DEBUG
}else{
    CONFIGURATION = Release
}

unix {
    PLATFORM = Linux64
}

DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore

HEADERS += \
    ISConfigurations.h \
    ISConsole.h \
    ISDatabase.h \
    ISQuery.h \
    isserver_global.h

SOURCES += \
    ISConfigurations.cpp \
    ISConsole.cpp \
    ISDatabase.cpp \
    ISQuery.cpp

RESOURCES += $$PWD/../../Resources/ISServer.qrc
