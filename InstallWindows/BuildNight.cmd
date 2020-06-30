@ECHO off
SetLocal

REM Скрипт принимает один единственный параметр в виде
REM пути к папке вида C:\Github\IntegralSystem
REM Если такая папка уже существует - она будет удалена

REM Пример запуска скрипта
REM BuildNight.cmd G:\Github\BuildNight

REM Если временная папка с проектом уже существует - удаляем её
IF EXIST %1 (RMDIR /S /Q %1)

REM Клонируем репозиторий из master-ветки и переходим в папку с выгруженным репозиторием
git clone -b master https://github.com/artshestakov/IntegralSystem.git %1

REM Затем переходим в папку InstallWindows
CD %1\InstallWindows

REM Делаем текущую папку рабочей (Working dir)
PUSHD %1\InstallWindows

REM Запускаем сборку в обоих конфигурациях
CALL _Build_IntegralSystem_Release_Win32.cmd
CALL _Build_Carat_Release_Win32.cmd

REM Получение текущей даты в переменную CURRENT_DATE
DATE /T > temp.tmp
SET /p CURRENT_DATE=<temp.tmp
DEL temp.tmp

REM Создание папки с текущей датой в облаке
IF NOT EXIST %USERPROFILE%\iCloudDrive\IntegralSystem\%CURRENT_DATE% (MKDIR %USERPROFILE%\iCloudDrive\IntegralSystem\%CURRENT_DATE%)

REM Копирование дистрибутивов в облако
COPY ..\Output\* %USERPROFILE%\iCloudDrive\IntegralSystem\%CURRENT_DATE%

REM После сборки выходим из папки репозитория (чтобы можно было удалить за собой папку)
CD ..
CD ..

REM Ставим на ожидание на 5 секунд
%1\Components\Sleep\Release-Win32\Sleep.exe 5

REM После сборки удаляем за собой временную папку
IF EXIST %1 (RMDIR /S /Q %1)