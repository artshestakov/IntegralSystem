TARGET = ISCore
TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
DEFINES += ISCORE_LIBRARY

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

LIBS += -lpthread

HEADERS += \
    ISAlgorithm.h \
    ISConstants.h \
    ISDebug.h \
    ISLogger.h \
    ISNamespace.h \
    ISStructs.h \
    ISTypedefs.h \
    StdAfx.h \
    iscore_global.h

SOURCES += \
    ISAlgorithm.cpp \
    ISDebug.cpp \
    ISLogger.cpp \
    StdAfx.cpp
