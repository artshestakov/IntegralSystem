#!/bin/bash

#Скрипт преназначен для сборки проекта или библиотеки
#Пример запуска скрипта ./BuildItem.sh Library ISCore Debug Linux64
#Где:
#Library - папка содержащая проект или библиотеку
#ISCore - имя проекта или библиотеки
#Debug - конфигурация сборки
#Linux64 - платформа сборки

#Подготовка переменных
DIR=$1
PROJECT_NAME=$2
CONFIGURATION=$3
PLATFORM=$4
IS_LIB=$5

#Удаление старых бинарников
#Если переменная $IS_LIB не заполнена - удаляем бинарник проекта, иначе - удаляем библиотеку и её симлинки)
if [ -z "$IS_LIB" ]
then
	rm -f ../Bin/$CONFIGURATION-$PLATFORM/$PROJECT_NAME
else
	rm -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so
	rm -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1
	rm -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1.0
	rm -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1.0.0
fi

#Сборка
$QT_DIR/bin/qmake ../${DIR}/$PROJECT_NAME/$PROJECT_NAME.pro -spec linux-g++ CONFIG+=${CONFIGURATION,,}
make -j$(nproc --all)

#Если сборка прошла успешно - копируем собранные бинарники в папку Deploy
if [ $? -eq 0 ]
then
	#Копирование скомпилированных файлов в папку Deploy
	#Если переменная $IS_LIB не заполнена - копируем только бинарный файл проекта, иначе - копируем библиотеку с её симлинками
	if [ -z "$IS_LIB" ]
	then
		cp -f ../Bin/$CONFIGURATION-$PLATFORM/$PROJECT_NAME ../Deploy/$CONFIGURATION-$PLATFORM
	else
		cp -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so ../Deploy/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so
		cp -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1 ../Deploy/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1
		cp -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1.0 ../Deploy/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1.0
		cp -f ../Bin/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1.0.0 ../Deploy/$CONFIGURATION-$PLATFORM/lib$PROJECT_NAME.so.1.0.0
	fi
fi
