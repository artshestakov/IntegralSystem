@ECHO off
SetLocal

REM Выполняем очистку решения
MSBuild ..\Solution.sln /p:Configuration=%1 /p:Platform=%2 /t:Clean

REM Запускаем сборку по обычному сценарию
call Build.cmd %1 %2 %3