QT += core gui xml widgets

TARGET = ISNetwork
TEMPLATE = lib
CONFIG += c++11 precompile_header
DEFINES += ISNETWORK_LIBRARY
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

INCLUDEPATH += $$PWD/../ISResources
INCLUDEPATH += $$PWD/../ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

SOURCES += \
    ISDaDataService.cpp \
    ISEmailAddress.cpp \
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
    ISOpenWeatherMap.cpp \
    ISQuotedPrintable.cpp \
    ISRestWorker.cpp \
    ISSmtpClient.cpp \
    ISCBR.cpp \
    ISAuthConnector.cpp

HEADERS += \
        isnetwork_global.h \ 
    ISDaDataService.h \
    ISEmailAddress.h \
    ISMimeAttachment.h \
    ISMimeContentFormatter.h \
    ISMimeFile.h \
    ISMimeHtml.h \
    ISMimeInlineFile.h \
    ISMimeMessage.h \
    ISMimeMultipart.h \
    ISMimePart.h \
    ISMimeText.h \
    ISNetwork.h \
    ISOpenWeatherMap.h \
    ISQuotedPrintable.h \
    ISRestWorker.h \
    ISSmtpClient.h \
    ISCBR.h \
    ISAuthConnector.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
