@ECHO off
SetLocal

call vcvars32.bat
call WinDeployClear.cmd %1 %2

git rev-list HEAD --count > ..\Resources\Version\Revision.txt
git rev-parse HEAD > ..\Resources\Version\Hash.txt
ECHO %DATE% > ..\Resources\Version\FromDate.txt
ECHO %TIME:~0,2%:%TIME:~3,2% > ..\Resources\Version\FromTime.txt
ECHO %2 > ..\Resources\Version\Platform.txt

IF EXIST IntegralSystem.iss.tmp DEL IntegralSystem.iss.tmp

REM Формирование версии
ECHO Versioning...
SET /p MAJOR=<..\Resources\Version\Major.txt
SET /p MINOR=<..\Resources\Version\Minor.txt
SET /p REVISION=<..\Resources\Version\Revision.txt
SET /p BUILD=<..\Resources\Version\Build.txt
ECHO Versioning DONE

REM Запрос на очистку решения перед сборкой
SET /p "QuestionRebuild=Rebuild? (y/n):"

IF /i "%QuestionRebuild%"=="Y" (
MSBuild ..\Solution.sln /p:Configuration=%1 /p:Platform=%2 /p:PlatformToolset=%3 /t:Clean
) ELSE (
ECHO Rebuild not start
)

REM Запуск сборки
MSBuild IntegralSystem.proj /p:Configuration=%1 /p:Platform=%2 /p:PlatformToolset=%3 /p:Version=%MAJOR%.%MINOR%.%REVISION%.%BUILD%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK
