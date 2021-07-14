TARGET = Carat
TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -g

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
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../Components/Arguments
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include
INCLUDEPATH += $$PWD/../../Components/RapidJSON

LIBS += \
    -L$$DESTDIR \
	-L$$PWD/../../Components/PostgreSQL/12.0.5/Lib-$${PLATFORM} \
	-L$$PWD/../../Deploy/$${CONFIGURATION}-$${PLATFORM} \
	-Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore \
	-lISServer \
	-lpq \
	-luuid \
	-lcrypto \
	-lcurl

SOURCES += \
    ../../Components/Arguments/ISArguments.cpp \
    ISCaratApplication.cpp \
    ISCrashDumper.cpp \
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
    ../../Components/Arguments/ISArguments.h \
    ../../ISRevision.h \
    ISCaratApplication.h \
    ISCrashDumper.h \
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
