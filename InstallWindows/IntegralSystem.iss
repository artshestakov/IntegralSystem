[Setup]
AppId={{757D746C-0676-404E-BC4C-40246AB5E38}
AppName=IntegralSystem
AppVerName=IntegralSystem
AppVersion=.Version.
VersionInfoVersion=.Version.
DefaultDirName={sd}\IntegralSystem
DefaultGroupName=IntegralSystem
OutputDir=..\Output
OutputBaseFilename=IntegralSystem_.Configuration._.Platform._.Version.
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

[Types]
Name: Installation; Description: "Тип установки"; Flags: iscustom;

[Components]
Name: Server; Description: "Сервер"; Types: Installation; Flags: exclusive;
Name: Client; Description: "Клиент"; Types: Installation; Flags: exclusive;

[Icons]
Name: "{userdesktop}\Carat"; Filename: "{app}\Carat.exe"; WorkingDir: "{app}"; Tasks: DesktopIconServer; Comment: "Серверный процесс Carat";
Name: "{userdesktop}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Tasks: DesktopIconClient; Comment: "Система управления данными «IntegralSystem»";
Name: "{userdesktop}\Configurator"; Filename: "{app}\Configurator.exe"; WorkingDir: "{app}"; Tasks: DesktopIconConfigurator; Comment: "Утилита управления базой данных «IntegralSystem»";
Name: "{group}\Carat"; Filename: "{app}\Carat.exe"; WorkingDir: "{app}"; Tasks: DesktopIconServer; Comment: "Серверный процесс Carat";
Name: "{group}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Comment: "Система управления данными IntegralSystem";
Name: "{group}\Конфигуратор"; Filename: "{app}\Configurator.exe"; WorkingDir: "{app}"; Tasks: DesktopIconServer; Comment: "Интерпретатор базы данных IntegralSystem";
Name: "{group}\Удалить Integral System"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; Comment: "Удаление Integral System";

[Tasks]
Name: "DesktopIconServer"; Description: "Создать значок «Carat» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components: Server;
Name: "DesktopIconClient"; Description: "Создать значок «IntegralSystem» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components: Server Client;
Name: "DesktopIconConfigurator"; Description: "Создать значок «Configurator» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components: Server Client;

[Run]
Filename: "{app}\Carat.exe"; Description: "Запустить сервер после выхода из установщика"; Flags: postinstall shellexec skipifsilent; Components: Server;
Filename: "{app}\IntegralSystem.exe"; Description: "Запустить программу после выхода из установщика"; Flags: postinstall shellexec skipifsilent; Components: Server Client;

[Files]
Source: ..\Deploy\.Configuration.-.Platform.\*; Excludes: "*.exp, *.ilk, *.lib, *.pdb"; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Resources\Icons\IntegralSystem.ico; DestDir: {app}; Flags: ignoreversion; Components: Server Client;
Source: ..\InstallResources\Update.cmd; DestDir: {app}; Flags: ignoreversion; Components: Server Client;
Source: ..\Components\Redistributable\Redistributable_2015_.Platform..exe; DestDir: {app}; Flags: ignoreversion; Components: Server Client

[Code]
procedure InitializeWizard();
begin
  WizardForm.WelcomeLabel2.Caption := WizardForm.WelcomeLabel2.Caption + #13#10#13#10 + 
  'Конфигурация: .Configuration.' + #13#10 +
  'Платформа: .Platform.' + #13#10 +
  'Версия устанавливаемой программы: .Version.';
end;