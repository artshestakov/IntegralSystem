MSBuild ..\Library\%1\%1.vcxproj /p:Configuration=%2 /p:Platform=%3 /p:OutDir=..\..\Deploy\%2-%3\ /p:PlatformToolset=%4 /maxcpucount:8
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK