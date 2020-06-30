[Setup]
AppId={{952CCC6B-DBA2-431A-94B5-FA4BF434CB4D}
AppName=Carat
AppVerName=Carat
AppVersion=${VERSION}
VersionInfoVersion=${VERSION}
DefaultDirName={sd}\Carat
DefaultGroupName=Carat
OutputDir=..\Output
OutputBaseFilename=Carat_${CONFIGURATION}_${PLATFORM}_${VERSION}
AlwaysShowDirOnReadyPage=yes
ShowLanguageDialog=yes
DisableProgramGroupPage=no
AppPublisher=Shestakov Artem
CreateUninstallRegKey=yes
DirExistsWarning=auto
LicenseFile=..\Resources\Licenses\IntegralSystem.txt
WizardImageStretch=yes
SetupIconFile=..\Resources\Icons\Carat.ico
FlatComponentsList=no
ShowComponentSizes=yes
InfoBeforeFile=..\InstallResources\InformationBefore.txt
InfoAfterFile=..\InstallResources\InformationAfter.txt
Compression=lzma2/ultra64
SolidCompression=yes
PrivilegesRequired=admin
UninstallDisplayIcon={app}\Carat.ico
UninstallDisplayName=Carat
DisableWelcomePage=no

[Languages]
Name: "Russian"; MessagesFile: "compiler:Languages\Russian.isl";

[Tasks]
Name: "IconDesktop"; Description: "Создать значок «Configurator» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}";

[Icons]
Name: "{userdesktop}\Configurator"; Filename: "{app}\Configurator.exe"; WorkingDir: "{app}"; Tasks: IconDesktop; Comment: "Конфигуратор базы данных";
Name: "{group}\Configurator"; Filename: "{app}\Configurator.exe"; WorkingDir: "{app}"; Comment: "Конфигуратор базы данных";
Name: "{group}\Удалить Carat"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; Comment: "Удаление Carat";

[Run]
Filename: "{app}\Configurator.exe"; Description: "Запустить конфигуратор после выхода из установщика"; Flags: postinstall shellexec skipifsilent;

[Files]
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\ISCore.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\ISCarat.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\Carat.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\CaratWorker.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\Configurator.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;

Source: ${QTDIR}\bin\Qt5Core${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Network${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Sql${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Xml${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;

Source: ${QTDIR}\plugins\platforms\qwindows${IS_DEBUG}.dll; DestDir: {app}\platforms; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\plugins\sqldrivers\qsqlpsql${IS_DEBUG}.dll; DestDir: {app}\sqldrivers; Flags: ignoreversion recursesubdirs;

Source: ..\Components\PostgreSQL\9.4\Bin-${PLATFORM}\*; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Resources\Icons\Carat.ico; DestDir: {app}; Flags: ignoreversion recursesubdirs;
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