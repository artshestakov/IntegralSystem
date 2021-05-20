@ECHO off
SetLocal

REM ������� ������ ������

REM ���������� ���������� ��� �������
SET CONFIGURATION=%1
SET PLATFORM=%2
REM SET SCRIPT_NAME=%3
REM SET QT_PATH=%4
SET DEPLOY_DIR=..\Deploy\%CONFIGURATION%-%PLATFORM%
REM SET IS_DEBUG=""
SET /a CPU_CORE_COUNT=%NUMBER_OF_PROCESSORS% * 2

REM ����������� ���������� �����
CALL vcvars32.bat

REM ���������� ���������� Deploy (������� ������� ���� ����, ����� ������)
IF EXIST %DEPLOY_DIR% (RMDIR /S /Q %DEPLOY_DIR%)
MKDIR %DEPLOY_DIR%

REM �������� ����� ������� (���������� �������) � ���������� ���� ���������� � ������
REM git rev-list --count HEAD > revision.tmp
REM SET /p REVISION=< revision.tmp
REM DEL revision.tmp
REM CALL GenerateBuildInfo.cmd %CONFIGURATION% %PLATFORM% %REVISION%

MSBuild ..\SolutionSTD.sln /t:Rebuild /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /p:OutDir=..\..\Deploy\%CONFIGURATION%-%PLATFORM%\ -maxcpucount /m:%CPU_CORE_COUNT% /p:CL_MPCount=%CPU_CORE_COUNT%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK

REM ������� ����� ������
git rev-list --count HEAD > Revision.tmp
SET /p REVISION=<Revision.tmp
DEL Revision.tmp

REM ��������� ������ ������� ������ (������� ����������, ���������� ����� � ���������� ������)
IF EXIST Carat.iss.tmp (DEL Carat.iss.tmp)
COPY Carat.iss Carat.iss.tmp
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe Carat.iss.tmp ${CONFIGURATION} %CONFIGURATION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe Carat.iss.tmp ${PLATFORM} %PLATFORM%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe Carat.iss.tmp ${VERSION} %REVISION%

REM ������ ������
%INNO_SETUP% Carat.iss.tmp

REM �������� ����������� ������� � ������
REM IF "%SCRIPT_NAME%"=="IntegralSystem" (COPY ..\Output\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe C:\Users\%USERNAME%\YandexDisk\Client\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe)