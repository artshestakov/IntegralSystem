REM Скрипт преназначен для сборки проекта или библиотеки
REM Пример запуска скрипта BuildItem.cmd Library ISCore Debug Win32
REM Где:
REM Library - папка содержащая проект или библиотеку
REM ISCore - имя проекта или библиотеки
REM Debug - конфигурация сборки
REM Win32 - платформа сборки

MSBuild ..\%1\%2\%2.vcxproj -p:Configuration=%3 -p:Platform=%4 -p:OutDir=..\..\Deploy\%3-%4\ -maxcpucount /m:%NUMBER_OF_PROCESSORS% /p:CL_MPCount=%NUMBER_OF_PROCESSORS%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK