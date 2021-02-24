QT += core sql xml
QT -= gui

TARGET = Configurator
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
INCLUDEPATH += $$PWD/../../Components/OpenSSL/Include
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore \
	-lISServer
LIBS += -L$$PWD/../../Components/OpenSSL/Bin-$${PLATFORM} -lcrypto

SOURCES += \
    CGConfigurator.cpp \
    CGConfiguratorBase.cpp \
    CGConfiguratorCreate.cpp \
    CGConfiguratorDelete.cpp \
    CGConfiguratorService.cpp \
    CGConfiguratorShow.cpp \
    CGConfiguratorUpdate.cpp \
    CGDatabase.cpp \
    main.cpp

HEADERS += \
    CGConfigurator.h \
    CGConfiguratorBase.h \
    CGConfiguratorCreate.h \
    CGConfiguratorDelete.h \
    CGConfiguratorService.h \
    CGConfiguratorShow.h \
    CGConfiguratorUpdate.h \
    CGDatabase.h
