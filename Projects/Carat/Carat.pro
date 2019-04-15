QT += core sql network

TARGET = Carat
CONFIG += console c++11 precompile_header
TEMPLATE = app
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

INCLUDEPATH += $$PWD/../../Library/ISCarat
INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += $$PWD/../../Library/ISNetwork
INCLUDEPATH += $$PWD/../../Library/ISResources
INCLUDEPATH += $$PWD/../../Library/ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += $$PWD/!Service
INCLUDEPATH += $$PWD/!System
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="$${DESTDIR}",-rpath-link="$${DESTDIR}" \
    -lISCarat \
    -lISCore \
    -lISNetwork \
    -lISResources \
    -lISSystem \

SOURCES += !System/main.cpp \
    !Service/QtService.cpp \
    !Service/QtService_Unix.cpp \
    !System/ISCaratService.cpp \
    !Service/QtUnixServerSocket.cpp \
    !Service/QtUnixSocket.cpp \
    !System/ISCaratApplication.cpp \
    !System/ISProcessCore.cpp \
    ../../PublicSource/System/EXLicense.cpp

HEADERS += \
    !Service/QtService.h \
    !Service/QtService_p.h \
    !System/ISCaratService.h \
    !Service/QtUnixServerSocket.h \
    !Service/QtUnixSocket.h \
    !System/ISCaratApplication.h \
    !System/ISProcessCore.h \
    ../../PublicSource/System/EXLicense.h
