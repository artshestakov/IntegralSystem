MSBuild ..\Library\ISResources\ISResources.vcxproj /p:Configuration=%1 /p:Platform=%2 /p:OutDir=..\..\Deploy\%1-%2\ /p:PlatformToolset=%3 /maxcpucount:8 /t:Rebuild
IF ERRORLEVEL 1 GOTO ERROR
GOTO OK
:ERROR
PAUSE
:OK