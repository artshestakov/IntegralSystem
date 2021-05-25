TARGET = Carat
TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
	DEFINES += DEBUG
}else{
    CONFIGURATION = Release
}

unix {
    PLATFORM = Linux64
}

DESTDIR = $$PWD/../../BinSTD/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../../LibrarySTD/ISCore
INCLUDEPATH += $$PWD/../../LibrarySTD/ISServer
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include
INCLUDEPATH += $$PWD/../../Components/RapidJSON

LIBS += -L$$DESTDIR -L$$PWD/../../Components/PostgreSQL/12.0.5/Lib-$${PLATFORM} -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore \
	-lISServer \
	-lpq \
	-luuid \
	-lcrypto \
	-lcurl

SOURCES += \
    ISCaratApplication.cpp \
    ISFail2Ban.cpp \
    ISTcpAnswer.cpp \
    ISTcpClient.cpp \
    ISTcpClients.cpp \
    ISTcpMessage.cpp \
    ISTcpQueue.cpp \
    ISTcpServer.cpp \
    ISTcpWorker.cpp \
    ISTcpWorkerHelper.cpp \
    ISBlockedIP.cpp \
    main.cpp

HEADERS += \
    ../../ISRevision.h \
    ../../Revision.h \
    ISCaratApplication.h \
    ISFail2Ban.h \
    ISTcpAnswer.h \
    ISTcpClient.h \
    ISTcpClients.h \
    ISTcpMessage.h \
    ISTcpQueue.h \
    ISTcpServer.h \
    ISTcpWorker.h \
    ISTcpWorkerHelper.h \
    ISBlockedIP.h
