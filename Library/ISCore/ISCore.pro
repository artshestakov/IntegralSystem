QT += core network xml sql
QT -= gui

TARGET = ISCore
TEMPLATE = lib
CONFIG += c++11
DEFINES += ISCORE_LIBRARY

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
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
    ISApplicationRunning.h \
    ISAssert.h \
    ISAuthConnector.h \
    ISConfig.h \
    ISConsole.h \
    ISConstants.h \
    ISCore.h \
    ISTcp.h \
    ISTcpConnector.h \
    ISTcpQuery.h \
    iscore_global.h \
    ISCountingTime.h \
    ISCrashDumper.h \
    ISDatabase.h \
    ISDefinesCore.h \
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
    PMetaClass.h \
    StdAfx.h

SOURCES += \
    ISAlgorithm.cpp \
    ISApplicationRunning.cpp \
    ISAssert.cpp \
    ISAuthConnector.cpp \
    ISConfig.cpp \
    ISConsole.cpp \
    ISCore.cpp \
    ISCountingTime.cpp \
    ISCrashDumper.cpp \
    ISDatabase.cpp \
    ISDefinesCore.cpp \
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
    StdAfx.cpp \
    call_stack_gcc.cpp

RESOURCES += $$PWD/../../Resources/Resources.qrc
