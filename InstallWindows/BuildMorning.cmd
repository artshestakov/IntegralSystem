@ECHO off
SetLocal

REM ������ ��������� ���� ������������ �������� � ����
REM ���� � ������� ����� ������� ���� C:\Github\IntegralSystem

REM ��������� � ����� �� ����������� ���� � ������ � ������� (Working dir)
cd %1
pushd %1

REM �������� ������� ������ ��������� ���� �� �����������
git pull

call %1\InstallWindows\vcvars32.bat

REM ������ ������
MSBuild Solution.sln /t:Build /p:Configuration=Debug /p:Platform=Win32
MSBuild Solution.sln /t:Build /p:Configuration=Release /p:Platform=Win32