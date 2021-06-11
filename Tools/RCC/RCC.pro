QT -= core gui
TARGET = RCC
TEMPLATE = app
CONFIG += c++11

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
}else{
    CONFIGURATION = Release
}

DESTDIR = $$PWD/../Bin/$${CONFIGURATION}-Linux64
MOC_DIR = $$PWD/$${CONFIGURATION}-Linux64/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-Linux64/$${TARGET}

INCLUDEPATH += .

SOURCES += \
    main.cpp \
	System.h

