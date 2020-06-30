[Setup]
AppId={{757D746C-0676-404E-BC4C-40246AB5E38}
AppName=IntegralSystem
AppVerName=IntegralSystem
AppVersion=${VERSION}
VersionInfoVersion=${VERSION}
DefaultDirName={sd}\IntegralSystem
DefaultGroupName=IntegralSystem
OutputDir=..\Output
OutputBaseFilename=IntegralSystem_${CONFIGURATION}_${PLATFORM}_${VERSION}
AlwaysShowDirOnReadyPage=yes
ShowLanguageDialog=yes
DisableProgramGroupPage=no
AppPublisher=Shestakov Artem
CreateUninstallRegKey=yes
DirExistsWarning=auto
LicenseFile=..\Resources\Licenses\IntegralSystem.txt
WizardImageStretch=yes
SetupIconFile=..\Resources\Icons\IntegralSystem.ico
FlatComponentsList=no
ShowComponentSizes=yes
InfoBeforeFile=..\InstallResources\InformationBefore.txt
InfoAfterFile=..\InstallResources\InformationAfter.txt
Compression=lzma2/ultra64
SolidCompression=yes
PrivilegesRequired=admin
UninstallDisplayIcon={app}\IntegralSystem.ico
UninstallDisplayName=IntegralSystem
DisableWelcomePage=no

[Languages]
Name: "Russian"; MessagesFile: "compiler:Languages\Russian.isl";

[Tasks]
Name: "IconDesktop"; Description: "Создать значок «IntegralSystem» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}";

[Icons]
Name: "{userdesktop}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Tasks: IconDesktop; Comment: "Система управления данными «IntegralSystem»";
Name: "{group}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Comment: "Система управления данными IntegralSystem";
Name: "{group}\Удалить Integral System"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; Comment: "Удаление Integral System";

[Run]
Filename: "{app}\IntegralSystem.exe"; Description: "Запустить программу после выхода из установщика"; Flags: postinstall shellexec skipifsilent;

[Files]
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\ISCore.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\ISGui.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\IntegralSystem.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;

Source: ${QTDIR}\bin\d3dcompiler_47.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\libEGL${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\libGLESV2${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\opengl32sw.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Core${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Gui${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Multimedia${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Network${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5PrintSupport${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Sql${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Widgets${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Xml${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;

Source: ${QTDIR}\plugins\audio\qtaudio_wasapi${IS_DEBUG}.dll; DestDir: {app}\audio; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\audio\qtaudio_windows${IS_DEBUG}.dll; DestDir: {app}\audio; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\bearer\qgenericbearer${IS_DEBUG}.dll; DestDir: {app}\bearer; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\iconengines\qsvgicon${IS_DEBUG}.dll; DestDir: {app}\iconengines; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qgif${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qicns${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qico${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qjpeg${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qsvg${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qtga${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qtiff${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qwbmp${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\imageformats\qwebp${IS_DEBUG}.dll; DestDir: {app}\imageformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\mediaservice\dsengine${IS_DEBUG}.dll; DestDir: {app}\mediaservice; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\mediaservice\qtmedia_audioengine${IS_DEBUG}.dll; DestDir: {app}\mediaservice; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\mediaservice\wmfengine${IS_DEBUG}.dll; DestDir: {app}\mediaservice; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\platforms\qwindows${IS_DEBUG}.dll; DestDir: {app}\platforms; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\playlistformats\qtmultimedia_m3u${IS_DEBUG}.dll; DestDir: {app}\playlistformats; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\printsupport\windowsprintersupport${IS_DEBUG}.dll; DestDir: {app}\printsupport; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\sqldrivers\qsqlpsql${IS_DEBUG}.dll; DestDir: {app}\sqldrivers; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\styles\qwindowsvistastyle${IS_DEBUG}.dll; DestDir: {app}\styles; Flags: ignoreversion recursesubdirs;

Source: ..\Components\PostgreSQL\9.4\Bin-${PLATFORM}\*; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Resources\Icons\IntegralSystem.ico; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Components\Redistributable\Redistributable_2013_${PLATFORM}.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Components\Redistributable\Redistributable_2015_${PLATFORM}.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\translations\qt_ru.qm; DestDir: {app}\translations; Flags: ignoreversion recursesubdirs;

[Run]
Filename: {app}\Redistributable_2013_${PLATFORM}.exe; Description: "Установка VC++ Redistributable 2013"; Parameters: "/install /quiet"; WorkingDir: {app}; StatusMsg: "Установка VC++ Redistributable 2013...";
Filename: {app}\Redistributable_2015_${PLATFORM}.exe; Description: "Установка VC++ Redistributable 2015"; Parameters: "/install /quiet"; WorkingDir: {app}; StatusMsg: "Установка VC++ Redistributable 2015...";

[Code]
procedure InitializeWizard();
begin
  WizardForm.WelcomeLabel2.Caption := WizardForm.WelcomeLabel2.Caption + #13#10#13#10 + 
  'Конфигурация: ${CONFIGURATION}' + #13#10 +
  'Платформа: ${PLATFORM}' + #13#10 +
  'Версия устанавливаемой программы: ${VERSION}';
end;