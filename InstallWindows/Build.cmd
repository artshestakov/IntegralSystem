@ECHO off
SetLocal

call vcvars32.bat
call WinDeployClear.cmd %1 %2
call GenerateBuildInfo.cmd %1 %2

REM Если шаблон скрипта InnoSetup уже сущесвует - удаляем его
IF EXIST IntegralSystem.iss.tmp DEL IntegralSystem.iss.tmp

REM Запуск сборки
MSBuild IntegralSystem.proj /p:Configuration=%1 /p:Platform=%2 /p:PlatformToolset=%3 /p:Version=%MAJOR%.%MINOR%.%REVISION%.%BUILD%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK
