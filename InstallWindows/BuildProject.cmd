MSBuild ..\Projects\%1\%1.vcxproj -p:Configuration=%2 -p:Platform=%3 -p:OutDir=..\..\Deploy\%2-%3\ -p:PlatformToolset=%4 -maxcpucount /m:%NUMBER_OF_PROCESSORS% /p:CL_MPCount=%NUMBER_OF_PROCESSORS%
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK