REM Скрипт преназначен для сборки проекта или библиотеки
REM Пример запуска скрипта BuildItem.cmd Library ISCore Debug Win32
REM Где:
REM Library - папка содержащая проект или библиотеку
REM ISCore - имя проекта или библиотеки
REM Debug - конфигурация сборки
REM Win32 - платформа сборки

SET /a CPU_COUNT=%NUMBER_OF_PROCESSORS% * 2

MSBuild ..\%1\%2\%2.vcxproj -p:Configuration=%3 -p:Platform=%4 -p:OutDir=..\..\Deploy\%3-%4\ -maxcpucount /m:%CPU_COUNT% /p:CL_MPCount=%CPU_COUNT%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK