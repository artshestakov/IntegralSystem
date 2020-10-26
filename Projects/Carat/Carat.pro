QT += core network sql xml
QT -= gui

TARGET = Carat
TEMPLATE = app
CONFIG += c++11

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
    DEFINES += DEBUG
}else{
    CONFIGURATION = Release
}

unix {
    PLATFORM = Linux64
}

DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../../Library/ISCarat
INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore

HEADERS += \
    ISCaratApplication.h \
    ISCaratController.h \
    ISTcpAnswer.h \
    ISTcpMessage.h \
    ISTcpQueue.h \
    ISTcpServer.h \
    ISTcpSocket.h \
    ISTcpWorker.h

SOURCES += \
    ISCaratApplication.cpp \
    ISCaratController.cpp \
    ISTcpAnswer.cpp \
    ISTcpMessage.cpp \
    ISTcpQueue.cpp \
    ISTcpServer.cpp \
    ISTcpSocket.cpp \
    ISTcpWorker.cpp \
    main.cpp
