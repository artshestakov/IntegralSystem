@ECHO off
SetLocal

REM Получение текущей даты в переменную CURRENT_DATE
DATE /T > temp.tmp
SET /p CURRENT_DATE=<temp.tmp
DEL temp.tmp

REM Создание папки с текущей датой
IF NOT EXIST %USERPROFILE%\iCloudDrive\IntegralSystem\%CURRENT_DATE% (MKDIR %USERPROFILE%\iCloudDrive\IntegralSystem\%CURRENT_DATE%)

REM Копирование инсталлятора в облако
COPY ..\Output\IntegralSystem_%1_%2_%3.exe %USERPROFILE%\iCloudDrive\IntegralSystem\%CURRENT_DATE%