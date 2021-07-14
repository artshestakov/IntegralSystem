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

DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include

LIBS += \
    -L$$DESTDIR \
	-L$$PWD/../../Components/PostgreSQL/12.0.5/Lib-$${PLATFORM} \
	-L$$PWD/../../Deploy/$${CONFIGURATION}-$${PLATFORM} \
	-Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
	-lpq \
	-lISCore

HEADERS += \
    RCC.h \
    ISConfigurations.h \
    ISConsole.h \
    ISDatabase.h \
    ISQuery.h \
	ISQueryText.h \
	ISMetaData.h

SOURCES += \
    ISConfigurations.cpp \
    ISConsole.cpp \
    ISDatabase.cpp \
    ISQuery.cpp \
	ISQueryText.cpp \
	ISMetaData.cpp
