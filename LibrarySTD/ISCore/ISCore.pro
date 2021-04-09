TARGET = ISCore
TEMPLATE = lib
CONFIG += c++11 staticlib
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

LIBS += -lpthread -luuid

HEADERS += \
    ConvertUTF.h \
    ISAlgorithm.h \
    ISAssert.h \
    ISConfig.h \
    ISConstants.h \
    ISDateTime.h \
    ISDebug.h \
    ISLocalization.h \
    ISLogger.h \
    ISMetaData.h \
    ISNamespace.h \
    ISResourcer.h \
    ISStructs.h \
    ISTrace.h \
    ISTypedefs.h \
    PMetaClass.h \
    SimpleIni.h \
	StdAfx.h \
    tinyxml2.h

SOURCES += \
    ConvertUTF.c \
    ISAlgorithm.cpp \
    ISAssert.cpp \
    ISConfig.cpp \
    ISDateTime.cpp \
    ISDebug.cpp \
    ISLocalization.cpp \
    ISLogger.cpp \
    ISMetaData.cpp \
    ISResourcer.cpp \
    ISTrace.cpp \
    StdAfx.cpp \
    tinyxml2.cpp
