@ECHO off

SET SOLTION_DIR=%1
SET CONFIGURATION=%2
SET PLATFORM=%3

SET GET_UTIL_PATH=%SOLTION_DIR%Components\GET\Release-Win32\GET.exe

REM Получаем ревизию
git rev-list --count HEAD > %SOLTION_DIR%Revision.tmp
SET /p REVISION=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

REM Получаем имя текущей ветки
git rev-parse --abbrev-ref HEAD > %SOLTION_DIR%Revision.tmp
SET /p BRANCH_NAME=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

REM Получааем хэш текущего комита
git rev-parse --verify HEAD > %SOLTION_DIR%Revision.tmp
SET /p HASH=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

REM Получаем текущую дату
%GET_UTIL_PATH% --getdate > %SOLTION_DIR%Revision.tmp
SET /p CURRENT_DATE=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

REM Получаем текущее время
%GET_UTIL_PATH% --gettime > %SOLTION_DIR%Revision.tmp
SET /p CURRENT_TIME=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

REM Формируем заголовочный файл
ECHO #pragma once>>%SOLTION_DIR%Revision.tmp
ECHO #ifndef _ISREVISION_H_INCLUDED>>%SOLTION_DIR%Revision.tmp
ECHO #define _ISREVISION_H_INCLUDED>>%SOLTION_DIR%Revision.tmp
ECHO //----------------------------------------------------------------------------->>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_VERSION_N %REVISION%>>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_VERSION "%REVISION%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_CONFIGURATION "%CONFIGURATION%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_PLATFORM "%PLATFORM%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_BRANCH_NAME "%BRANCH_NAME%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_HASH "%HASH%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_BUILD_DATE "%CURRENT_DATE%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_BUILD_TIME "%CURRENT_TIME%">>%SOLTION_DIR%Revision.tmp
ECHO //----------------------------------------------------------------------------->>%SOLTION_DIR%Revision.tmp
ECHO #endif>>%SOLTION_DIR%Revision.tmp

REM Переименовываем
MOVE /Y %SOLTION_DIR%Revision.tmp %SOLTION_DIR%ISRevision.h
