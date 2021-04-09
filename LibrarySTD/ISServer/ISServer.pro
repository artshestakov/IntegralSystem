TARGET = ISServer
TEMPLATE = lib
CONFIG += c++11 staticlib
CONFIG -= app_bundle
CONFIG -= qt
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

DESTDIR = $$PWD/../../BinSTD/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../../LibrarySTD/ISCore
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include

LIBS += -L$$PWD/../../Components/PostgreSQL/12.0.5/Lib-$${PLATFORM} -lpq

HEADERS += \
    ISDatabase.h \
    ISQuery.h

SOURCES += \
    ISDatabase.cpp \
    ISQuery.cpp
