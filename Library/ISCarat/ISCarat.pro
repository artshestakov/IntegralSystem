QT += core network
QT -= gui

TARGET = ISCarat
TEMPLATE = lib
CONFIG += c++11
DEFINES += ISCARAT_LIBRARY

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
}else{
    CONFIGURATION = Release
}

unix {
    PLATFORM = Linux64
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
    ISCoreMedTech.h \
    ISCoreTelephony.h \
    iscarat_global.h

SOURCES += \
    ISAsteriskSocket.cpp \
    ISCaratCoreApplication.cpp \
    ISCoreAsteriskQueue.cpp \
    ISCoreMedTech.cpp \
    ISCoreTelephony.cpp
