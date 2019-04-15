QT += core network

TARGET = ISCarat
TEMPLATE = lib
CONFIG += c++11 precompile_header
DEFINES += ISCARAT_LIBRARY
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
INCLUDEPATH += $$PWD/../ISNetwork
INCLUDEPATH += $$PWD/../ISResources
INCLUDEPATH += $$PWD/../ISSystem
INCLUDEPATH += $$PWD/../../PublicSource/System
INCLUDEPATH += .

SOURCES += \
    ISCoreNotification.cpp \
    ISCoreTelephony.cpp \
    ISCoreCalendar.cpp \
    ISCoreAsteriskRecord.cpp \
    ISAsteriskSocket.cpp \
    ISCaratCoreApplication.cpp \
    ISCoreAsteriskQueue.cpp \
    ISCoreHighway.cpp \
    ISCoreInformResource.cpp \
    ISCoreMedTech.cpp \
    ISCoreCenterSeven.cpp \
    ISCoreScheduler.cpp \
    ISCoreMail.cpp \
    ISCoreSMS.cpp

HEADERS += \
    ISCoreNotification.h \
    ISCoreTelephony.h \
    ISCoreCalendar.h \
    ISCoreAsteriskRecord.h \
    ISAsteriskSocket.h \
    ISCaratCoreApplication.h \
    ISCoreAsteriskQueue.h \
    ISCoreHighway.h \
    ISCoreInformResource.h \
    ISCoreMedTech.h \
    ISCoreCenterSeven.h \
    ISCoreScheduler.h \
    ISCoreMail.h \
    ISCoreSMS.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
