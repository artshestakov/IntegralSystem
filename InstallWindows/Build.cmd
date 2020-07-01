@ECHO off
SetLocal

REM Главный скрипт сборки

REM Настройка переменных среды
CALL vcvars32.bat

REM Подготовка переменных для скрипта
SET CONFIGURATION=%1
SET PLATFORM=%2
SET SCRIPT_NAME=%3
SET MAJOR="4"
SET MINOR="9"
SET DEPLOY_DIR=..\Deploy\%CONFIGURATION%-%PLATFORM%
SET IS_DEBUG=""

REM Если текущая сборка отладочная - заполняем переменную IS_DEBUG символом "d"
IF "%CONFIGURATION%" == "Debug" (SET "IS_DEBUG=d")

REM Подготовка директории Deploy (сначала удаляем если есть, затем создаём)
IF EXIST %DEPLOY_DIR% (RMDIR /S /Q %DEPLOY_DIR%)
MKDIR %DEPLOY_DIR%

REM Получаем номер ревизии (количество комитов) и генерируем файл информации о сборке
git rev-list --all --count > revision.tmp
SET /p REVISION=< revision.tmp
DEL revision.tmp
CALL GenerateBuildInfo.cmd %CONFIGURATION% %PLATFORM% %MAJOR% %MINOR% %REVISION%

REM Компиляция библиотек
CALL BuildItem.cmd Library ISCore %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Library ISGui %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Library ISCarat %CONFIGURATION% %PLATFORM%

REM Компиляция проектов
CALL BuildItem.cmd Projects Carat %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects CaratWorker %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects Configurator %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects IntegralSystem %CONFIGURATION% %PLATFORM%

REM Формируем шаблон скрипта сборки (удаляем предыдущий, генерируем новый и производим замены)
IF EXIST %SCRIPT_NAME%.iss.tmp (DEL %SCRIPT_NAME%.iss.tmp)
COPY %SCRIPT_NAME%.iss %SCRIPT_NAME%.iss.tmp
..\Components\Replacer\Release-Win32\Replacer.exe %SCRIPT_NAME%.iss.tmp ${CONFIGURATION} %CONFIGURATION%
..\Components\Replacer\Release-Win32\Replacer.exe %SCRIPT_NAME%.iss.tmp ${PLATFORM} %PLATFORM%
..\Components\Replacer\Release-Win32\Replacer.exe %SCRIPT_NAME%.iss.tmp ${VERSION} %MAJOR%.%MINOR%.%REVISION%
..\Components\Replacer\Release-Win32\Replacer.exe %SCRIPT_NAME%.iss.tmp ${QTDIR} %QTDIR%
..\Components\Replacer\Release-Win32\Replacer.exe %SCRIPT_NAME%.iss.tmp ${IS_DEBUG} %IS_DEBUG%

REM Генерируем трансляции (создаём папку для них и генерируем)
MKDIR %DEPLOY_DIR%\translations
%QTDIR%\bin\lconvert.exe -o %DEPLOY_DIR%\translations\qt_ru.qm %QTDIR%\translations\assistant_ru.qm %QTDIR%\translations\designer_ru.qm %QTDIR%\translations\linguist_ru.qm %QTDIR%\translations\qmlviewer_ru.qm %QTDIR%\translations\qtbase_ru.qm %QTDIR%\translations\qtconnectivity_ru.qm %QTDIR%\translations\qtdeclarative_ru.qm %QTDIR%\translations\qtlocation_ru.qm ^%QTDIR%\translations\qtmultimedia_ru.qm %QTDIR%\translations\qtquick1_ru.qm %QTDIR%\translations\qtquickcontrols_ru.qm %QTDIR%\translations\qtscript_ru.qm %QTDIR%\translations\qtserialport_ru.qm %QTDIR%\translations\qtwebengine_ru.qm %QTDIR%\translations\qtwebsockets_ru.qm %QTDIR%\translations\qtxmlpatterns_ru.qm

REM Запуск сборки
%INNO_SETUP% %SCRIPT_NAME%.iss.tmp