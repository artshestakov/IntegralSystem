@ECHO off
SetLocal

REM ��������� ������� �������
MSBuild ..\Solution.sln /p:Configuration=%1 /p:Platform=%2 /t:Clean

REM ��������� ������ �� �������� ��������
call Build.cmd %1 %2 %3