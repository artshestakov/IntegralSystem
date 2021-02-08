#!/bin/bash

#Главный скрипт сборки

export INTEGRAL_SYSTEM_DIR="$(pwd)"/..
export QT_DIR="/home/artem/Qt/5.11.3/gcc_64"

#Подготовка переменных для скрипта
CONFIGURATION=$1
PLATFORM=Linux64
DEPLOY_DIR=../Deploy/$CONFIGURATION-Linux64

#Подготовка директории Deploy (сначала удаляем если есть, затем создаём)
if [ -d "$DEPLOY_DIR" ]
then
	rm -R $DEPLOY_DIR
fi
mkdir $DEPLOY_DIR

#Получаем номер ревизии (количество комитов) и генерируем файл информации о сборке
REVISION=$(git rev-list --count HEAD)
./GenerateBuildInfo.sh $CONFIGURATION $PLATFORM $REVISION

#Компиляция библиотек
./BuildItem.sh Library ISCore $CONFIGURATION $PLATFORM lib
./BuildItem.sh Library ISServer $CONFIGURATION $PLATFORM lib

#Компиляция проектов
./BuildItem.sh Projects Carat $CONFIGURATION $PLATFORM
./BuildItem.sh Projects Configurator $CONFIGURATION $PLATFORM

#Формируем шаблон скрипта сборки (удаляем предыдущий, генерируем новый и производим замены)
if [ -f "./IntegralSystem.spec.tmp" ]
then
	rm -f ./IntegralSystem.spec.tmp
fi
cp -f ./IntegralSystem.spec ./IntegralSystem.spec.tmp
../Components/Replacer/$CONFIGURATION-Linux64/Replacer ./IntegralSystem.spec.tmp %{CONFIGURATION} $CONFIGURATION
../Components/Replacer/$CONFIGURATION-Linux64/Replacer ./IntegralSystem.spec.tmp %{PLATFORM} $PLATFORM
../Components/Replacer/$CONFIGURATION-Linux64/Replacer ./IntegralSystem.spec.tmp %{VERSION} $REVISION

#Сборка RPM-пакета
rpmbuild -bb IntegralSystem.spec.tmp --define "_rpmdir $INTEGRAL_SYSTEM_DIR"

#Копируем в облако
cp ../x86_64/IntegralSystem-$REVISION-1.x86_64.rpm ../../Yandex.Disk/Server/
