@ECHO OFF
SetLocal

REM Скрипт преназначен для генерации информации о сборке

REM Путь к файлу
SET FILE_PATH=..\Resources\Build\Build.ini.tmp
SET GET_UTIL_PATH=..\Components\GET\Release-Win32\GET.exe

REM Если такой файл уже существует - удаляем его
IF EXIST %FILE_PATH% (DEL %FILE_PATH%)

REM Получаем текущую дату
%GET_UTIL_PATH% --getdate > temp.tmp
SET /p CURRENT_DATE=<temp.tmp
DEL temp.tmp

REM Получаем текущее время
%GET_UTIL_PATH% --gettime > temp.tmp
SET /p CURRENT_TIME=<temp.tmp
DEL temp.tmp

REM Получааем хэш текущего комита
git rev-parse --verify HEAD > temp.tmp
SET /p HASH=<temp.tmp
DEL temp.tmp

REM Получаем имя текущей ветки
git rev-parse --abbrev-ref HEAD > temp.tmp
SET /p BRANCH_NAME=<temp.tmp
DEL temp.tmp

REM Пишем информацию в файл
ECHO [Version]>> %FILE_PATH%
ECHO Major="%3">> %FILE_PATH%
ECHO Minor="%4">> %FILE_PATH%
ECHO Revision="%5">> %FILE_PATH%
ECHO.>> %FILE_PATH%
ECHO [Build]>> %FILE_PATH%
ECHO Date="%CURRENT_DATE%">> %FILE_PATH%
ECHO Time="%CURRENT_TIME%">> %FILE_PATH%
ECHO Hash="%HASH%">> %FILE_PATH%
ECHO Branch="%BRANCH_NAME%">> %FILE_PATH%
ECHO Configuration="%1">> %FILE_PATH%
ECHO Platform="%2">> %FILE_PATH%