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

INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += $$PWD/../../Library/ISServer
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore \
	-lISServer

HEADERS += \
    ISAsterisk.h \
    ISCaratApplication.h \
    ISCaratMonitor.h \
    ISFail2Ban.h \
    ISTcpAnswer.h \
    ISTcpClients.h \
    ISTcpMessage.h \
    ISTcpQueue.h \
    ISTcpServer.h \
    ISTcpSocket.h \
    ISTcpWorker.h \
	ISOilSphereWorker.h \
    ISTcpWorkerHelper.h

SOURCES += \
    ISAsterisk.cpp \
    ISCaratApplication.cpp \
    ISCaratMonitor.cpp \
    ISFail2Ban.cpp \
    ISTcpAnswer.cpp \
    ISTcpClients.cpp \
    ISTcpMessage.cpp \
    ISTcpQueue.cpp \
    ISTcpServer.cpp \
    ISTcpSocket.cpp \
    ISTcpWorker.cpp \
	ISOilSphereWorker.cpp \
    ISTcpWorkerHelper.cpp \
	main.cpp
