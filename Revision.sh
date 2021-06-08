#!/bin/bash

ECHO Generate "ISRevision.h"...

SOLUTION_DIR=$1
CONFIGURATION=$2
PLATFORM=$3

GET_UTIL_PATH="$SOLUTION_DIR"Components/GET/Release-Linux64/GET

#Получаем ревизию
REVISION=$(git rev-list --count HEAD)

#Получаем имя текущей ветки
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)

#Получааем хэш текущего комита
HASH=$(git rev-parse --verify HEAD)

#Получаем текущую дату
CURRENT_DATE=$($GET_UTIL_PATH --getdate)

#Получаем текущее время
CURRENT_TIME=$($GET_UTIL_PATH --gettime)

#Формируем заголовочный файл
echo "#pragma once" >> "$SOLUTION_DIR"Revision.tmp
echo "#ifndef _ISREVISION_H_INCLUDED" >> "$SOLUTION_DIR"Revision.tmp
echo "#define _ISREVISION_H_INCLUDED" >> "$SOLUTION_DIR"Revision.tmp
echo "//-----------------------------------------------------------------------------" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_VERSION_N $REVISION" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_VERSION \"$REVISION\"" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_CONFIGURATION \"$CONFIGURATION\"" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_PLATFORM \"$PLATFORM\"" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_BRANCH_NAME \"$BRANCH_NAME\"" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_HASH \"$HASH\"" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_BUILD_DATE \"$CURRENT_DATE\"" >> "$SOLUTION_DIR"Revision.tmp
echo "#define CARAT_BUILD_TIME \"$CURRENT_TIME\"" >> "$SOLUTION_DIR"Revision.tmp
echo "//-----------------------------------------------------------------------------" >> "$SOLUTION_DIR"Revision.tmp
echo "#endif" >> "$SOLUTION_DIR"Revision.tmp

#Переименовываем
mv -f "$SOLUTION_DIR"Revision.tmp "$SOLUTION_DIR"ISRevision.h
