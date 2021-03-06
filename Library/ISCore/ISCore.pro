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

DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

LIBS += -lpthread -luuid -lcrypto

RevisionTarget.commands = $$PWD/../../Revision.sh $$PWD/../../ $${CONFIGURATION} $${PLATFORM}
QMAKE_EXTRA_TARGETS += RevisionTarget
PRE_TARGETDEPS += RevisionTarget

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
    ISNamespace.h \
    ISProperty.h \
    ISResourcer.h \
    ISStructs.h \
    ISTrace.h \
    ISTypedefs.h \
    PMetaClass.h \
    SimpleIni.h \
	StdAfx.h \
    hashmap.h \
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
    ISProperty.cpp \
    ISResourcer.cpp \
    ISTrace.cpp \
    StdAfx.cpp \
    hashmap.cpp \
    tinyxml2.cpp
