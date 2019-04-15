QT += core sql xml

TARGET = ISConfigurator
TEMPLATE = lib
CONFIG += c++11 precompile_header
DEFINES += ISCONFIGURATOR_LIBRARY
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

INCLUDEPATH += $$PWD/../ISCore
INCLUDEPATH += $$PWD/../ISResources
INCLUDEPATH += $$PWD/../ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

SOURCES += \ 
    CGClass.cpp \
    CGClassField.cpp \
    CGConfigurator.cpp \
    CGConfiguratorBase.cpp \
    CGConfiguratorCreate.cpp \
    CGConfiguratorDelete.cpp \
    CGConfiguratorService.cpp \
    CGConfiguratorShow.cpp \
    CGConfiguratorUpdate.cpp \
    CGForeign.cpp \
    CGFunction.cpp \
    CGHelper.cpp \
    CGIndex.cpp \
    CGResource.cpp \
    CGSection.cpp \
    CGSectionItem.cpp \
    CGSequence.cpp \
    CGTable.cpp \
    CGTemplateField.cpp \
    CGConfiguratorLicense.cpp \
    CGConfiguratorFIAS.cpp

HEADERS += \ 
    CGClass.h \
    CGClassField.h \
    CGConfigurator.h \
    CGConfiguratorBase.h \
    CGConfiguratorCreate.h \
    CGConfiguratorDelete.h \
    CGConfiguratorService.h \
    CGConfiguratorShow.h \
    CGConfiguratorUpdate.h \
    CGForeign.h \
    CGFunction.h \
    CGHelper.h \
    CGIndex.h \
    CGResource.h \
    CGSection.h \
    CGSectionItem.h \
    CGSequence.h \
    CGTable.h \
    CGTemplateField.h \
    isconfigurator_global.h \
    CGConfiguratorLicense.h \
    CGConfiguratorFIAS.h
