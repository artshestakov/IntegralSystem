@ECHO off
SetLocal

REM Настройка переменных среды
CALL vcvars32.bat

REM Подготовка переменных для скрипта
SET CONFIGURATION=%1
SET PLATFORM=%2
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
CALL BuildLibrary.cmd ISCore %CONFIGURATION% %PLATFORM%
CALL BuildLibrary.cmd ISGui %CONFIGURATION% %PLATFORM%
CALL BuildLibrary.cmd ISCarat %CONFIGURATION% %PLATFORM%

REM Компиляция проектов
CALL BuildProject.cmd Carat %CONFIGURATION% %PLATFORM%
CALL BuildProject.cmd CaratWorker %CONFIGURATION% %PLATFORM%
CALL BuildProject.cmd Configurator %CONFIGURATION% %PLATFORM%
CALL BuildProject.cmd IntegralSystem %CONFIGURATION% %PLATFORM%

REM Формируем шаблон скрипта сборки (удаляем предыдущий, генерируем новый и производим замены)
IF EXIST IntegralSystem.iss.tmp (DEL IntegralSystem.iss.tmp)
COPY IntegralSystem.iss IntegralSystem.iss.tmp
..\Components\Replacer\Release-Win32\Replacer.exe IntegralSystem.iss.tmp ${CONFIGURATION} %CONFIGURATION%
..\Components\Replacer\Release-Win32\Replacer.exe IntegralSystem.iss.tmp ${PLATFORM} %PLATFORM%
..\Components\Replacer\Release-Win32\Replacer.exe IntegralSystem.iss.tmp ${VERSION} %MAJOR%.%MINOR%.%REVISION%
..\Components\Replacer\Release-Win32\Replacer.exe IntegralSystem.iss.tmp ${QTDIR} %QTDIR%
..\Components\Replacer\Release-Win32\Replacer.exe IntegralSystem.iss.tmp ${IS_DEBUG} %IS_DEBUG%

REM Генерируем трансляции (создаём папку для них и генерируем)
MKDIR %DEPLOY_DIR%\translations
%QTDIR%\bin\lconvert.exe -o %DEPLOY_DIR%\translations\qt_ru.qm %QTDIR%\translations\assistant_ru.qm %QTDIR%\translations\designer_ru.qm %QTDIR%\translations\linguist_ru.qm %QTDIR%\translations\qmlviewer_ru.qm %QTDIR%\translations\qtbase_ru.qm %QTDIR%\translations\qtconnectivity_ru.qm %QTDIR%\translations\qtdeclarative_ru.qm %QTDIR%\translations\qtlocation_ru.qm ^%QTDIR%\translations\qtmultimedia_ru.qm %QTDIR%\translations\qtquick1_ru.qm %QTDIR%\translations\qtquickcontrols_ru.qm %QTDIR%\translations\qtscript_ru.qm %QTDIR%\translations\qtserialport_ru.qm %QTDIR%\translations\qtwebengine_ru.qm %QTDIR%\translations\qtwebsockets_ru.qm %QTDIR%\translations\qtxmlpatterns_ru.qm

REM Запуск сборки
%INNO_SETUP% IntegralSystem.iss.tmp