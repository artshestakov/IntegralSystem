@ECHO off

SET Configuration=%1
SET Platform=%2
SET Folder=%Configuration%-%Platform%

setlocal enabledelayedexpansion

FOR /F "usebackq" %%a IN (`DIR /B ..\Deploy\%Folder%\*.dll ..\Deploy\%Folder%\*.exe`) DO (
SET Result=%%a
SET Path=%cd..%..\Deploy\%1-%2\!Result!

%QTDIR%\bin\windeployqt.exe !Path! --no-angle --no-opengl-sw --no-system-d3d-compiler --release-with-debug-info --pdb --no-translations --%Configuration% --libdir ..\Deploy\%Folder% --dir ..\Deploy\%Folder%
)

MKDIR ..\Deploy\%Folder%\translations
%QTDIR%\bin\lconvert.exe -o ..\Deploy\%Folder%\translations\qt_ru.qm %QTDIR%\translations\assistant_ru.qm %QTDIR%\translations\designer_ru.qm %QTDIR%\translations\linguist_ru.qm %QTDIR%\translations\qmlviewer_ru.qm %QTDIR%\translations\qtbase_ru.qm %QTDIR%\translations\qtconfig_ru.qm %QTDIR%\translations\qtconnectivity_ru.qm %QTDIR%\translations\qtdeclarative_ru.qm %QTDIR%\translations\qtlocation_ru.qm %QTDIR%\translations\qtmultimedia_ru.qm %QTDIR%\translations\qtquick1_ru.qm %QTDIR%\translations\qtquickcontrols_ru.qm %QTDIR%\translations\qtscript_ru.qm %QTDIR%\translations\qtserialport_ru.qm %QTDIR%\translations\qtwebengine_ru.qm %QTDIR%\translations\qtwebsockets_ru.qm %QTDIR%\translations\qtxmlpatterns_ru.qm