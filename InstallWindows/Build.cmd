@ECHO off
SetLocal

REM Главный скрипт сборки

REM Настройка переменных среды
CALL vcvars32.bat

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

REM Компиляция библиотек
CALL BuildItem.cmd Library ISCore %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Library ISServer %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Library ISGui %CONFIGURATION% %PLATFORM%

REM Компиляция проектов
CALL BuildItem.cmd Projects Carat %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects Configurator %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects IntegralSystem %CONFIGURATION% %PLATFORM%

REM Формируем шаблон скрипта сборки (удаляем предыдущий, генерируем новый и производим замены)
IF EXIST %SCRIPT_NAME%.iss.tmp (DEL %SCRIPT_NAME%.iss.tmp)
COPY %SCRIPT_NAME%.iss %SCRIPT_NAME%.iss.tmp
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${CONFIGURATION} %CONFIGURATION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${PLATFORM} %PLATFORM%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${VERSION} %REVISION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${QTDIR} %QT_PATH%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${IS_DEBUG} %IS_DEBUG%

REM Генерируем трансляции (создаём папку для них и генерируем)
MKDIR %DEPLOY_DIR%\translations
%QT_PATH%\bin\lconvert.exe -o %DEPLOY_DIR%\translations\qt_ru.qm %QT_PATH%\translations\assistant_ru.qm %QT_PATH%\translations\designer_ru.qm %QT_PATH%\translations\linguist_ru.qm %QT_PATH%\translations\qmlviewer_ru.qm %QT_PATH%\translations\qtbase_ru.qm %QT_PATH%\translations\qtconnectivity_ru.qm %QT_PATH%\translations\qtdeclarative_ru.qm %QT_PATH%\translations\qtlocation_ru.qm ^%QT_PATH%\translations\qtmultimedia_ru.qm %QT_PATH%\translations\qtquick1_ru.qm %QT_PATH%\translations\qtquickcontrols_ru.qm %QT_PATH%\translations\qtscript_ru.qm %QT_PATH%\translations\qtserialport_ru.qm %QT_PATH%\translations\qtwebengine_ru.qm %QT_PATH%\translations\qtwebsockets_ru.qm %QT_PATH%\translations\qtxmlpatterns_ru.qm

REM Запуск сборки
%INNO_SETUP% %SCRIPT_NAME%.iss.tmp

REM Копируем дистрибутив в облако
copy ..\Output\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe C:\Users\%USERNAME%\YandexDisk\Client\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe