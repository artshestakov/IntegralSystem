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
    ISAuthConnector.h \
    ISConfig.h \
    ISConstants.h \
    ISCore.h \
    ISDebug.h \
    ISTcp.h \
    ISTcpConnector.h \
    ISTcpQuery.h \
    iscore_global.h \
    ISDatabase.h \
    ISLocalization.h \
    ISLogger.h \
    ISMetaData.h \
    ISMetaDataHelper.h \
    ISMetaUser.h \
    ISMetaUuidCheckeder.h \
    ISNamespace.h \
    ISNetwork.h \
    ISProperty.h \
    ISProtocol.h \
    ISQuery.h \
    ISQueryModel.h \
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
    ISVersion.h \
    ISVersionInfo.h \
    PMetaClass.h \
    StdAfx.h

SOURCES += \
    ISAlgorithm.cpp \
    ISAssert.cpp \
    ISAuthConnector.cpp \
    ISConfig.cpp \
    ISCore.cpp \
    ISDatabase.cpp \
    ISDebug.cpp \
    ISLocalization.cpp \
    ISLogger.cpp \
    ISMetaData.cpp \
    ISMetaDataHelper.cpp \
    ISMetaUser.cpp \
    ISMetaUuidCheckeder.cpp \
    ISNetwork.cpp \
    ISProperty.cpp \
    ISProtocol.cpp \
    ISQuery.cpp \
    ISQueryModel.cpp \
    ISQueryPool.cpp \
    ISQueryText.cpp \
    ISRandom.cpp \
    ISRestWorker.cpp \
    ISSettings.cpp \
    ISSettingsDatabase.cpp \
    ISSystem.cpp \
    ISTcp.cpp \
    ISTcpConnector.cpp \
    ISTcpQuery.cpp \
    ISTrace.cpp \
    ISUuid.cpp \
    ISVersion.cpp \
    ISVersionInfo.cpp \
    StdAfx.cpp

RESOURCES += $$PWD/../../Resources/ISCore.qrc
