QT += core gui xml

TARGET = ISResources
TEMPLATE = lib
CONFIG += c++11 precompile_header
DEFINES += ISRESOURCES_LIBRARY
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
RCC_DIR = $$PWD/../../Build/$${TARGET}/$${CONFIGURATION}_Linux64
PRECOMPILED_DIR = $$PWD/../../PrecompiledHeaders/$${TARGET}_$${CONFIGURATION}_Linux64

INCLUDEPATH += $$PWD/../ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

SOURCES += \
    ISLocalization.cpp \
    ISBuffer.cpp \
    ISStyleSheet.cpp

HEADERS += \
        isresources_global.h \
    ISLocalization.h \
    ISBuffer.h \
    ISStyleSheet.h

RESOURCES = $$PWD/../../Resources/Carat.qrc

unix {
    target.path = /usr/lib
    INSTALLS += target
}
