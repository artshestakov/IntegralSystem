@ECHO off
SetLocal

REM Главный скрипт сборки

REM Настройка переменных среды
REM CALL vcvars32.bat

REM Подготовка переменных для скрипта
SET CONFIGURATION=%1
SET PLATFORM=%2
SET SCRIPT_NAME=%3
SET QT_PATH=%4
SET DEPLOY_DIR=..\Deploy\%CONFIGURATION%-%PLATFORM%
SET IS_DEBUG=""
SET /a CPU_CORE_COUNT=%NUMBER_OF_PROCESSORS% * 2

REM Если текущая сборка отладочная - заполняем переменную IS_DEBUG символом "d"
IF "%CONFIGURATION%" == "Debug" (SET "IS_DEBUG=d")

REM Подготовка директории Deploy (сначала удаляем если есть, затем создаём)
IF EXIST %DEPLOY_DIR% (RMDIR /S /Q %DEPLOY_DIR%)
MKDIR %DEPLOY_DIR%

REM Получаем номер ревизии (количество комитов) и генерируем файл информации о сборке
git rev-list --count HEAD > revision.tmp
SET /p REVISION=< revision.tmp
DEL revision.tmp
CALL GenerateBuildInfo.cmd %CONFIGURATION% %PLATFORM% %REVISION%

REM Если переменная %CPU_CORE_COUNT% не заполнена - заполняем её фактическим количеством процессоров на текущей машине
IF "%CPU_CORE_COUNT%"=="" (SET /a CPU_CORE_COUNT=%NUMBER_OF_PROCESSORS%)

MSBuild ..\Solution.sln -p:Configuration=%CONFIGURATION% -p:Platform=%PLATFORM% -p:OutDir=..\..\Deploy\%CONFIGURATION%-%PLATFORM%\ -maxcpucount /m:%CPU_CORE_COUNT% /p:CL_MPCount=%CPU_CORE_COUNT%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK

REM Формируем шаблон скрипта сборки (удаляем предыдущий, генерируем новый и производим замены)
IF EXIST %SCRIPT_NAME%.iss.tmp (DEL %SCRIPT_NAME%.iss.tmp)
COPY %SCRIPT_NAME%.iss %SCRIPT_NAME%.iss.tmp
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${CONFIGURATION} %CONFIGURATION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${PLATFORM} %PLATFORM%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${VERSION} %REVISION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${QTDIR} %QT_PATH%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${IS_DEBUG} %IS_DEBUG%

REM Запуск сборки
%INNO_SETUP% %SCRIPT_NAME%.iss.tmp

REM Копируем дистрибутив клиента в облако
IF "%SCRIPT_NAME%"=="IntegralSystem" (COPY ..\Output\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe C:\Users\%USERNAME%\YandexDisk\Client\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe)