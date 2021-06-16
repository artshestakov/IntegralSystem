QT -= core gui
TARGET = RCC
TEMPLATE = app
CONFIG += c++11

CONFIG(debug, debug|release){
    CONFIGURATION = Debug
}else{
    CONFIGURATION = Release
}

DESTDIR = $$PWD/../../Components/$${TARGET}/$${CONFIGURATION}-Linux64
MOC_DIR = $$PWD/$${CONFIGURATION}-Linux64/$${TARGET}
OBJECTS_DIR = $$PWD/$${CONFIGURATION}-Linux64/$${TARGET}

INCLUDEPATH += $$PWD/../../Components/Arguments
INCLUDEPATH += .

HEADERS += \
    ../../Components/Arguments/ISArguments.h \
    System.h

SOURCES += \
    ../../Components/Arguments/ISArguments.cpp \
    main.cpp
