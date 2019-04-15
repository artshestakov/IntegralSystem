QT += core serialport

TARGET = ISCore
TEMPLATE = lib
CONFIG += c++11 precompile_header
DEFINES += ISCORE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
}else{
    CONFIGURATION = Release
}

PRECOMPILED_HEADER = $$PWD/../../PublicSource/System/StdAfx.h
DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-Linux64
MOC_DIR = $$PWD/../../Build/$${TARGET}/$${CONFIGURATION}_Linux64
OBJECTS_DIR = $$PWD/../../Build/$${TARGET}/$${CONFIGURATION}_Linux64
PRECOMPILED_DIR = $$PWD/../../PrecompiledHeaders/$${TARGET}_$${CONFIGURATION}_Linux64

INCLUDEPATH += $$PWD/../../Library/ISResources
INCLUDEPATH += $$PWD/../../Library/ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

SOURCES += \
    call_stack_gcc.cpp \
    ISAssociationTypes.cpp \
    ISCore.cpp \
    ISCrashDumper.cpp \
    ISDatabase.cpp \
    ISDatabaseHelper.cpp \
    ISLicense.cpp \
    ISLicenseItem.cpp \
    ISMemoryObjects.cpp \
    ISMetaData.cpp \
    ISMetaDataHelper.cpp \
    ISMetaNotify.cpp \
    ISMetaSetting.cpp \
    ISMetaSettingsGroup.cpp \
    ISMetaUser.cpp \
    ISMetaUserData.cpp \
    ISMetaUuidCheckeder.cpp \
    ISMetaViewQuery.cpp \
    ISModelThreadQuery.cpp \
    ISModelThreadWorker.cpp \
    ISNotifySender.cpp \
    ISProtocol.cpp \
    ISQuery.cpp \
    ISQueryModel.cpp \
    ISQueryModelHelper.cpp \
    ISQueryPool.cpp \
    ISQueryPoolObject.cpp \
    ISQueryText.cpp \
    ISSearchModel.cpp \
    ISSettings.cpp \
    ISSettingsDatabase.cpp \
    ISSqlModelCore.cpp \
    ISSqlModelHelper.cpp \
    ISSqlModelView.cpp \
    ISStorageFileLoader.cpp \
    PMetaClass.cpp \
    PMetaClassEscort.cpp \
    PMetaClassField.cpp \
    PMetaClassForeign.cpp \
    PMetaClassFunction.cpp \
    PMetaClassIndex.cpp \
    PMetaClassQuery.cpp \
    PMetaClassResource.cpp \
    PMetaClassTab.cpp \
    PMetaClassTable.cpp \
    ../../PublicSource/System/EXLicense.cpp \
    ISEMail.cpp \
    ISSMS.cpp

HEADERS += \
    call_stack.hpp \
    ISAssociationTypes.h \
    ISCore.h \
    iscore_global.h \
    ISCrashDumper.h \
    ISDatabase.h \
    ISDatabaseHelper.h \
    ISLicense.h \
    ISLicenseItem.h \
    ISMemoryObjects.h \
    ISMetaData.h \
    ISMetaDataHelper.h \
    ISMetaNotify.h \
    ISMetaSetting.h \
    ISMetaSettingsGroup.h \
    ISMetaUser.h \
    ISMetaUserData.h \
    ISMetaUuidCheckeder.h \
    ISMetaViewQuery.h \
    ISModelThreadQuery.h \
    ISModelThreadWorker.h \
    ISNotifySender.h \
    ISProtocol.h \
    ISQuery.h \
    ISQueryModel.h \
    ISQueryModelHelper.h \
    ISQueryPool.h \
    ISQueryPoolObject.h \
    ISQueryText.h \
    ISSearchModel.h \
    ISSettings.h \
    ISSettingsDatabase.h \
    ISSqlModelCore.h \
    ISSqlModelHelper.h \
    ISSqlModelView.h \
    ISStorageFileLoader.h \
    PMetaClass.h \
    PMetaClassEscort.h \
    PMetaClassField.h \
    PMetaClassForeign.h \
    PMetaClassFunction.h \
    PMetaClassIndex.h \
    PMetaClassQuery.h \
    PMetaClassResource.h \
    PMetaClassTab.h \
    PMetaClassTable.h \
    ../../PublicSource/System/EXLicense.h \
    ISEMail.h \
    ISSMS.h
