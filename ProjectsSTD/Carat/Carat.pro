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
INCLUDEPATH += $$PWD/../../Components/PostgreSQL/12.0.5/Include
INCLUDEPATH += $$PWD/../../Components/RapidJSON

LIBS += -L$$DESTDIR -Wl,-rpath="'\$$ORIGIN'",-rpath-link="'\$$ORIGIN'" \
    -lISCore \
	-lISServer

SOURCES += \
    ISCaratApplication.cpp \
    main.cpp

HEADERS += \
    ISCaratApplication.h
