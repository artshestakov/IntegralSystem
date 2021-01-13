QT += core network xml sql
QT -= gui

TARGET = ISCore
TEMPLATE = lib
CONFIG += c++11
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

INCLUDEPATH += .

HEADERS += \
    ISAlgorithm.h \
    ISAssert.h \
    ISConfig.h \
    ISConsole.h \
    ISConstants.h \
    ISCore.h \
    ISDebug.h \
    ISTcp.h \
    iscore_global.h \
    ISDatabase.h \
    ISLocalization.h \
    ISLogger.h \
    ISMetaData.h \
	ISMetaDataHelper.h \
    ISNamespace.h \
    ISProperty.h \
    ISQuery.h \
    ISQueryPool.h \
    ISQueryText.h \
    ISRandom.h \
    ISRestWorker.h \
    ISSettings.h \
    ISSettingsDatabase.h \
    ISStructs.h \
    ISSystem.h \
    ISTrace.h \
    ISTypedefs.h \
    ISUuid.h \
    ISVersionInfo.h \
    PMetaClass.h \
    StdAfx.h

SOURCES += \
    ISAlgorithm.cpp \
    ISAssert.cpp \
    ISConfig.cpp \
    ISConsole.cpp \
    ISCore.cpp \
    ISDatabase.cpp \
    ISDebug.cpp \
    ISLocalization.cpp \
    ISLogger.cpp \
    ISMetaData.cpp \
    ISMetaDataHelper.cpp \
    ISProperty.cpp \
    ISQuery.cpp \
    ISQueryPool.cpp \
    ISQueryText.cpp \
    ISRandom.cpp \
    ISRestWorker.cpp \
    ISSettings.cpp \
    ISSettingsDatabase.cpp \
    ISSystem.cpp \
    ISTcp.cpp \
    ISTrace.cpp \
    ISUuid.cpp \
    ISVersionInfo.cpp \
    StdAfx.cpp

RESOURCES += $$PWD/../../Resources/ISCore.qrc
