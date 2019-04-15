call vcvars32.bat
MSBuild ..\Projects\Licenser\Licenser.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:OutDir=..\..\Licenser /t:Rebuild