@ECHO off
SetLocal

REM Скрипт принимает один единственный параметр в виде
REM пути к папке вида C:\Github\IntegralSystem
REM Если такая папка уже существует - она будет удалена

REM Если временная папка с проектом уже существует - удаляем её
IF EXIST %1 (RMDIR /S /Q %1)

REM Клонируем репозиторий из master-ветки и переходим в папку с выгруженным репозиторием
git clone -b master https://github.com/artshestakov/IntegralSystem.git %1

REM Затем переходим в папку InstallWindows
cd %1\InstallWindows

REM Делаем текущую папку рабочей (Working dir)
pushd %1\InstallWindows

REM Запускаем сборку в обоих конфигурациях
call _Build_IntegralSystem_Debug_Win32.cmd
call _Build_IntegralSystem_Release_Win32.cmd

REM После сборки выходим из папки репозитория (чтобы можно было удалить за собой папку)
cd ..
cd ..

REM После сборки удаляем за собой временную папку
IF EXIST %1 (RMDIR /S /Q %1)