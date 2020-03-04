@ECHO OFF

REM Путь к файлу
SET FilePath=..\Resources\Build\Build.ini.tmp

REM Если такой файл уже существует - удаляем его
IF EXIST %FilePath% (DEL %FilePath%)

REM Получаем текущую дату
..\Components\GET\Release-Win32\GET.exe --getdate > temp.tmp
SET /p CURRENT_DATE=<temp.tmp
DEL temp.tmp

REM Получаем текущее время
..\Components\GET\Release-Win32\GET.exe --gettime > temp.tmp
SET /p CURRENT_TIME=<temp.tmp
DEL temp.tmp

REM Получааем хэш текущего комита
git rev-parse --verify HEAD > temp.tmp
SET /p HASH=<temp.tmp
DEL temp.tmp

REM Получаем имя текущей ветки
git rev-parse --abbrev-ref HEAD > temp.tmp
SET /p BRANCH=<temp.tmp
DEL temp.tmp

REM Пишем информацию в файл
ECHO [Version]>> %FilePath%
ECHO Major="%3">> %FilePath%
ECHO Minor="%4">> %FilePath%
ECHO Revision="%5">> %FilePath%
ECHO.>> %FilePath%
ECHO [Build]>> %FilePath%
ECHO Date="%CURRENT_DATE%">> %FilePath%
ECHO Time="%CURRENT_TIME%">> %FilePath%
ECHO Hash="%HASH%">> %FilePath%
ECHO Branch="%BRANCH%">> %FilePath%
ECHO Configuration="%1">> %FilePath%
ECHO Platform="%2">> %FilePath%