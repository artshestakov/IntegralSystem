QT += core gui xml widgets

TARGET = ISSystem
TEMPLATE = lib
CONFIG += c++11 precompile_header
DEFINES += ISSYSTEM_LIBRARY
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

INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

SOURCES += \
    ISSystem.cpp \
    ISCountingTime.cpp \
    ../../PublicSource/System/StdAfx.cpp \
    ISCommandLine.cpp \
    ISProperty.cpp \
    ISTrace.cpp \
    ISUuid.cpp \
    ISConfig.cpp \
    ISPassword.cpp \
    ISPhoneNumberParser.cpp \
    ISLogger.cpp \
    ISDebug.cpp \
    ISAssert.cpp \
    ISExceptionBase.cpp \
    ISQueryException.cpp \
    ISQueryExceptionConnection.cpp \
    ISQueryExceptionSyntax.cpp \
    ISQueryExceptionTransaction.cpp \
    ISProcess.cpp \
    ISTypes.cpp \
    ISExceptionAssert.cpp \
    ISApplicationRunning.cpp \
    ISRandom.cpp \
    ISVersion.cpp

HEADERS += \
        issystem_global.h \ 
    ISSystem.h \
    ISCountingTime.h \
    ../../PublicSource/System/StdAfx.h \
    ../../PublicSource/System/EXDefines.h \
    ISCommandLine.h \
    ISNamespace.h \
    ISProperty.h \
    ISTrace.h \
    ISUuid.h \
    ISConfig.h \
    ISPassword.h \
    ISPhoneNumberParser.h \
    ISLogger.h \
    ISDebug.h \
    ISAssert.h \
    ISExceptionBase.h \
    ISQueryException.h \
    ISQueryExceptionConnection.h \
    ISQueryExceptionSyntax.h \
    ISQueryExceptionTransaction.h \
    ISProcess.h \
    ISTypes.h \
    ISExceptionAssert.h \
    ISApplicationRunning.h \
    ISRandom.h \
    ISVersion.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
