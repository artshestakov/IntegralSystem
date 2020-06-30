#!/bin/bash

export INTEGRAL_SYSTEM_DIR="$(pwd)"/..
export QT_DIR="/home/artem/Qt/5.11.3/gcc_64"

#Подготовка переменных для скрипта
CONFIGURATION=$1
PLATFORM=Linux64
MAJOR=4
MINOR=9
DEPLOY_DIR=../Deploy/$CONFIGURATION-Linux64

#Подготовка директории Deploy (сначала удаляем если есть, затем создаём)
if [ -d "$DEPLOY_DIR" ]
then
	rm -R $DEPLOY_DIR
fi
mkdir $DEPLOY_DIR

#Получаем номер ревизии (количество комитов) и генерируем файл информации о сборке
REVISION=$(git rev-list --all --count)
./GenerateBuildInfo.sh $CONFIGURATION $PLATFORM $MAJOR $MINOR $REVISION

#Компиляция библиотек
./BuildItem.sh Library ISCore $CONFIGURATION $PLATFORM lib
./BuildItem.sh Library ISCarat $CONFIGURATION $PLATFORM lib

#Компиляция проектов
./BuildItem.sh Projects Carat $CONFIGURATION $PLATFORM
./BuildItem.sh Projects CaratWorker $CONFIGURATION $PLATFORM
./BuildItem.sh Projects Configurator $CONFIGURATION $PLATFORM

#Формируем шаблон скрипта сборки (удаляем предыдущий, генерируем новый и производим замены)
if [ -f "./IntegralSystem.spec.tmp" ]
then
	rm -f ./IntegralSystem.spec.tmp
fi
cp -f ./IntegralSystem.spec ./IntegralSystem.spec.tmp
../Components/Replacer/Release-Linux64/Replacer ./IntegralSystem.spec.tmp %{CONFIGURATION} $CONFIGURATION
../Components/Replacer/Release-Linux64/Replacer ./IntegralSystem.spec.tmp %{PLATFORM} $PLATFORM
../Components/Replacer/Release-Linux64/Replacer ./IntegralSystem.spec.tmp %{VERSION} $MAJOR.$MINOR.$REVISION

#Генерируем трансляции (создаём папку для них и генерируем)
mkdir $DEPLOY_DIR/translations
$QT_DIR/bin/lconvert -o ../Deploy/$CONFIGURATION-$PLATFORM/translations/qt_ru.qm $(ls -d -1 "$QT_DIR/"translations/* | grep _ru.qm)

#Сборка RPM-пакета
rpmbuild -bb IntegralSystem.spec.tmp --define "_rpmdir $INTEGRAL_SYSTEM_DIR"
