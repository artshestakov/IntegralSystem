@ECHO off
SetLocal

REM ������� ������ ������

REM ��������� ���������� �����
REM CALL vcvars32.bat

REM ���������� ���������� ��� �������
SET CONFIGURATION=%1
SET PLATFORM=%2
SET SCRIPT_NAME=%3
SET QT_PATH=%4
SET DEPLOY_DIR=..\Deploy\%CONFIGURATION%-%PLATFORM%
SET IS_DEBUG=""
SET /a CPU_CORE_COUNT=%NUMBER_OF_PROCESSORS% * 2

REM ���� ������� ������ ���������� - ��������� ���������� IS_DEBUG �������� "d"
IF "%CONFIGURATION%" == "Debug" (SET "IS_DEBUG=d")

REM ���������� ���������� Deploy (������� ������� ���� ����, ����� ������)
IF EXIST %DEPLOY_DIR% (RMDIR /S /Q %DEPLOY_DIR%)
MKDIR %DEPLOY_DIR%

REM �������� ����� ������� (���������� �������) � ���������� ���� ���������� � ������
git rev-list --count HEAD > revision.tmp
SET /p REVISION=< revision.tmp
DEL revision.tmp
CALL GenerateBuildInfo.cmd %CONFIGURATION% %PLATFORM% %REVISION%

REM ���������� ���������
CALL BuildItem.cmd Library ISCore %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Library ISServer %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Library ISGui %CONFIGURATION% %PLATFORM%

REM ���������� ��������
CALL BuildItem.cmd Projects Carat %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects Configurator %CONFIGURATION% %PLATFORM%
CALL BuildItem.cmd Projects IntegralSystem %CONFIGURATION% %PLATFORM%

REM ��������� ������ ������� ������ (������� ����������, ���������� ����� � ���������� ������)
IF EXIST %SCRIPT_NAME%.iss.tmp (DEL %SCRIPT_NAME%.iss.tmp)
COPY %SCRIPT_NAME%.iss %SCRIPT_NAME%.iss.tmp
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${CONFIGURATION} %CONFIGURATION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${PLATFORM} %PLATFORM%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${VERSION} %REVISION%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${QTDIR} %QT_PATH%
..\Components\Replacer\%CONFIGURATION%-%PLATFORM%\Replacer.exe %SCRIPT_NAME%.iss.tmp ${IS_DEBUG} %IS_DEBUG%

REM ������ ������
%INNO_SETUP% %SCRIPT_NAME%.iss.tmp

REM �������� ����������� � ������
copy ..\Output\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe C:\Users\%USERNAME%\YandexDisk\Client\IntegralSystem_%CONFIGURATION%_%PLATFORM%_%REVISION%.exe