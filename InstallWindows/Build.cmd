@ECHO off
SetLocal

call vcvars32.bat
call WinDeployClear.cmd %1 %2

REM ��������� �������� � �������� ������
SET MAJOR="4"
SET MINOR="9"

REM �������� ����� ������� (���������� �������)
git rev-list --all --count > temp.tmp
SET /p REVISION=< temp.tmp
DEL temp.tmp

call GenerateBuildInfo.cmd %1 %2 %MAJOR% %MINOR% %REVISION%

REM ���� ������ ������� InnoSetup ��� ��������� - ������� ���
IF EXIST IntegralSystem.iss.tmp DEL IntegralSystem.iss.tmp

REM ������ ������
MSBuild IntegralSystem.proj /p:Configuration=%1 /p:Platform=%2 /p:Version=%MAJOR%.%MINOR%.%REVISION%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK
