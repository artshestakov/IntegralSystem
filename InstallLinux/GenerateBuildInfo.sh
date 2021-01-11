#!/bin/bash

# Скрипт преназначен для генерации информации о сборке

#Пути к файлам
FILE_PATH=../Resources/Build/Build.ini.tmp
GET_UTIL_PATH=../Components/GET/Release-Linux64/GET

#Если такой файл уже существует - удаляем его
if [ -f "$FILE_PATH" ]; then
	rm $FILE_PATH
fi

#Получаем текущую дату
CURRENT_DATE=$($GET_UTIL_PATH --getdate)

#Получаем текущее время
CURRENT_TIME=$($GET_UTIL_PATH --gettime)

#Получааем хэш текущего комита
HASH=$(git rev-parse --verify HEAD)

#Получаем имя текущей ветки
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)

#Пишем информацию в файл
echo [Version] >> $FILE_PATH
echo Number="$3">> $FILE_PATH
echo >> $FILE_PATH
echo [Build]>> $FILE_PATH
echo Date="$CURRENT_DATE">> $FILE_PATH
echo Time="$CURRENT_TIME">> $FILE_PATH
echo Hash="$HASH">> $FILE_PATH
echo BranchName="$BRANCH_NAME">> $FILE_PATH
echo Configuration="$1">> $FILE_PATH
echo Platform="$2">> $FILE_PATH

