QT += core sql
QT -= gui

TARGET = ISServer
TEMPLATE = lib
CONFIG += c++11
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
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore
LIBS += -L$$PWD/../../Components/PostgreSQL/12.0.5/Lib-$${PLATFORM} -lpq

HEADERS += \
    ISConfigurations.h \
    ISConsole.h \
    ISDatabase.h \
    ISQuery.h \
    ISQueryLibPQ.h \
    isserver_global.h

SOURCES += \
    ISConfigurations.cpp \
    ISConsole.cpp \
    ISDatabase.cpp \
    ISQuery.cpp \
    ISQueryLibPQ.cpp

RESOURCES += $$PWD/../../Resources/ISServer.qrc
