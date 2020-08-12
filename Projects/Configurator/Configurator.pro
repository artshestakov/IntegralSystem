QT += core sql xml
QT -= gui

TARGET = Configurator
TEMPLATE = app
CONFIG += c++11

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

INCLUDEPATH += $$PWD/../../Library/ISCore
INCLUDEPATH += .

LIBS += -L$$DESTDIR -Wl,-rpath="$${DESTDIR}",-rpath-link="$${DESTDIR}" \
    -lISCore

SOURCES += \
    CGConfigurator.cpp \
    CGConfiguratorBase.cpp \
    CGConfiguratorCreate.cpp \
    CGConfiguratorDelete.cpp \
    CGConfiguratorService.cpp \
    CGConfiguratorShow.cpp \
    CGConfiguratorUpdate.cpp \
    CGForeign.cpp \
    CGFunction.cpp \
    CGHelper.cpp \
    CGIndex.cpp \
    CGResource.cpp \
    CGSequence.cpp \
    CGTable.cpp \
    CGTemplateField.cpp \
    main.cpp

HEADERS += \
    CGConfigurator.h \
    CGConfiguratorBase.h \
    CGConfiguratorCreate.h \
    CGConfiguratorDelete.h \
    CGConfiguratorService.h \
    CGConfiguratorShow.h \
    CGConfiguratorUpdate.h \
    CGForeign.h \
    CGFunction.h \
    CGHelper.h \
    CGIndex.h \
    CGResource.h \
    CGSequence.h \
    CGTable.h \
    CGTemplateField.h
