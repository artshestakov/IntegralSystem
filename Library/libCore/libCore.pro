QT += core network xml
QT -= gui

TARGET = libCore
TEMPLATE = lib
CONFIG += c++11
DEFINES += LIBCORE_LIBRARY

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
}else{
    CONFIGURATION = Release
}

unix {
    PLATFORM = Linux64
}

win32 {
    PLATFORM = Win32
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
    ISCountingTime.h \
    ISCrashDumper.h \
    ISDatabase.h \
    ISDefinesCore.h \
    ISEMail.h \
    ISEmailAddress.h \
    ISException.h \
    ISHTML.h \
    ISLocalization.h \
    ISLogger.h \
    ISMemoryObjects.h \
    ISMetaData.h \
    ISMetaDataHelper.h \
    ISMetaUser.h \
    ISMetaUuidCheckeder.h \
    ISMetaViewQuery.h \
    ISMimeAttachment.h \
    ISMimeContentFormatter.h \
    ISMimeFile.h \
    ISMimeHtml.h \
    ISMimeInlineFile.h \
    ISMimeMessage.h \
    ISMimeMultipart.h \
    ISMimePart.h \
    ISMimeText.h \
    ISNamespace.h \
    ISNetwork.h \
    ISNotifySender.h \
    ISPhoneNumberParser.h \
    ISProperty.h \
    ISProtocol.h \
    ISQuery.h \
    ISQueryModel.h \
    ISQueryModelHelper.h \
    ISQueryPool.h \
    ISQueryText.h \
    ISQuotedPrintable.h \
    ISRandom.h \
    ISRestWorker.h \
    ISSMS.h \
    ISSearchModel.h \
    ISSettings.h \
    ISSettingsDatabase.h \
    ISSmtpClient.h \
    ISStorageFileLoader.h \
    ISStructs.h \
    ISSystem.h \
    ISTrace.h \
    ISTypedefs.h \
    ISTypes.h \
    ISUuid.h \
    ISVersion.h \
    PMetaClass.h \
    StdAfx.h \
    call_stack.hpp \
    libCore_global.h

SOURCES += \
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
    ISEMail.cpp \
    ISEmailAddress.cpp \
    ISException.cpp \
    ISLocalization.cpp \
    ISLogger.cpp \
    ISMemoryObjects.cpp \
    ISMetaData.cpp \
    ISMetaDataHelper.cpp \
    ISMetaUser.cpp \
    ISMetaUuidCheckeder.cpp \
    ISMetaViewQuery.cpp \
    ISMimeAttachment.cpp \
    ISMimeContentFormatter.cpp \
    ISMimeFile.cpp \
    ISMimeHtml.cpp \
    ISMimeInlineFile.cpp \
    ISMimeMessage.cpp \
    ISMimeMultipart.cpp \
    ISMimePart.cpp \
    ISMimeText.cpp \
    ISNetwork.cpp \
    ISNotifySender.cpp \
    ISPhoneNumberParser.cpp \
    ISProperty.cpp \
    ISProtocol.cpp \
    ISQuery.cpp \
    ISQueryModel.cpp \
    ISQueryModelHelper.cpp \
    ISQueryPool.cpp \
    ISQueryText.cpp \
    ISQuotedPrintable.cpp \
    ISRandom.cpp \
    ISRestWorker.cpp \
    ISSMS.cpp \
    ISSearchModel.cpp \
    ISSettings.cpp \
    ISSettingsDatabase.cpp \
    ISSmtpClient.cpp \
    ISStorageFileLoader.cpp \
    ISSystem.cpp \
    ISTrace.cpp \
    ISUuid.cpp \
    ISVersion.cpp \
    StdAfx.cpp \
    call_stack_gcc.cpp
