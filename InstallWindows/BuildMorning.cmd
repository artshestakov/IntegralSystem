@ECHO off
SetLocal

REM Скрипт принимает один единственный параметр в виде
REM пути к рабочей папке проекта вида C:\Github\IntegralSystem

REM Пример запуска скрипта
REM BuildMorning.cmd G:\Github\IntegralSystem

REM Переходим в папку по переданному пути и делаем её рабочей (Working dir)
cd %1
pushd %1

REM Получаем свежусь версию исходного кода из репозитория
git pull

call %1\InstallWindows\vcvars32.bat

REM Запуск сборки
MSBuild Solution.sln /t:Rebuild /p:Configuration=Debug /p:Platform=Win32
MSBuild Solution.sln /t:Rebuild /p:Configuration=Release /p:Platform=Win32