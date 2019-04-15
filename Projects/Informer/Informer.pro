QT += core network

TARGET = Informer
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

INCLUDEPATH += $$PWD/../../Library/ISNetwork
INCLUDEPATH += $$PWD/../../Library/ISResources
INCLUDEPATH += $$PWD/../../Library/ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="$${DESTDIR}",-rpath-link="$${DESTDIR}" \
    -lISNetwork \
    -lISResources \
    -lISSystem \

SOURCES += main.cpp
