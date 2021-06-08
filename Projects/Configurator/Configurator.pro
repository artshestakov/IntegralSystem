TARGET = Configurator
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

DESTDIR = $$PWD/../../Bin/$${CONFIGURATION}-$${PLATFORM}
MOC_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-$${PLATFORM}/$${TARGET}

INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += $$PWD/../../Library/ISServer
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

HEADERS += \
    ../../ISRevision.h \
    CGConfigurator.h \
    CGConfiguratorBase.h \
    CGConfiguratorCreate.h \
    CGConfiguratorDelete.h \
    CGConfiguratorService.h \
    CGConfiguratorShow.h \
    CGConfiguratorUpdate.h \
    CGDatabase.h

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
