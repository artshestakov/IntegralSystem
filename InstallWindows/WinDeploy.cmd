@ECHO off

SET FileName=%1
SET Configuration=%2
SET Platform=%3

..\Components\StringToLower\Release-Win32\StringToLower.exe %Configuration% > temp.tmp
SET /p ConfigurationLower=<temp.tmp
DEL temp.tmp

%QTDIR%\bin\windeployqt.exe ..\Deploy\%Configuration%-%Platform%\%FileName% --no-translations --force -%ConfigurationLower% --libdir ..\Deploy\%Configuration%-%Platform% --dir ..\Deploy\%Configuration%-%Platform%

IF NOT EXIST ..\Deploy\%Configuration%-%Platform%\translations (MKDIR ..\Deploy\%Configuration%-%Platform%\translations)
%QTDIR%\bin\lconvert.exe -o ..\Deploy\%Folder%\translations\qt_ru.qm %QTDIR%\translations\assistant_ru.qm %QTDIR%\translations\designer_ru.qm %QTDIR%\translations\linguist_ru.qm %QTDIR%\translations\qmlviewer_ru.qm %QTDIR%\translations\qtbase_ru.qm %QTDIR%\translations\qtconnectivity_ru.qm %QTDIR%\translations\qtdeclarative_ru.qm %QTDIR%\translations\qtlocation_ru.qm %QTDIR%\translations\qtmultimedia_ru.qm %QTDIR%\translations\qtquick1_ru.qm %QTDIR%\translations\qtquickcontrols_ru.qm %QTDIR%\translations\qtscript_ru.qm %QTDIR%\translations\qtserialport_ru.qm %QTDIR%\translations\qtwebengine_ru.qm %QTDIR%\translations\qtwebsockets_ru.qm %QTDIR%\translations\qtxmlpatterns_ru.qm

REM Удаляем vcredist_x86
IF EXIST ..\Deploy\%Configuration%-%Platform%\vcredist_x86.exe (DEL ..\Deploy\%Configuration%-%Platform%\vcredist_x86.exe)