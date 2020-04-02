QT += core network
QT -= gui

TARGET = ISCarat
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

win32 {
    PLATFORM = Win32
}

DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../ISCore
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="$${DESTDIR}",-rpath-link="$${DESTDIR}" \
    -lISCore

HEADERS += \
    ISAsteriskSocket.h \
    ISCaratCoreApplication.h \
    ISCoreAsteriskQueue.h \
    ISCoreAsteriskRecord.h \
    ISCoreCalendar.h \
    ISCoreCenterSeven.h \
    ISCoreHighway.h \
    ISCoreInformResource.h \
    ISCoreMail.h \
    ISCoreMedTech.h \
    ISCoreNotification.h \
    ISCoreSMS.h \
    ISCoreScheduler.h \
    ISCoreTelephony.h \
    iscarat_global.h

SOURCES += \
    ISAsteriskSocket.cpp \
    ISCaratCoreApplication.cpp \
    ISCoreAsteriskQueue.cpp \
    ISCoreAsteriskRecord.cpp \
    ISCoreCalendar.cpp \
    ISCoreCenterSeven.cpp \
    ISCoreHighway.cpp \
    ISCoreInformResource.cpp \
    ISCoreMail.cpp \
    ISCoreMedTech.cpp \
    ISCoreNotification.cpp \
    ISCoreSMS.cpp \
    ISCoreScheduler.cpp \
    ISCoreTelephony.cpp
