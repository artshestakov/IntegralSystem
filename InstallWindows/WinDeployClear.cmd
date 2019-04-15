IF EXIST ..\Deploy DEL /S /Q ..\Deploy\*.exe
IF EXIST ..\Deploy DEL /S /Q ..\Deploy\*.dll
IF EXIST ..\Deploy DEL /S /Q ..\Deploy\*.pdb
IF EXIST ..\Deploy DEL /S /Q ..\Deploy\*.ilk
IF EXIST ..\Deploy DEL /S /Q ..\Deploy\*.exp
IF EXIST ..\Deploy DEL /S /Q ..\Deploy\*.lib

for /d %%i in ("..\Deploy\*") do rmdir /s /q "%%i"