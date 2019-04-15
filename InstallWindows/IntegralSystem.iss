[Setup]
AppId={{757D746C-0676-404E-BC4C-40246AB5E38}
AppName=IntegralSystem
AppVerName=IntegralSystem
AppVersion=%Version%
VersionInfoVersion=%Version%
DefaultDirName={sd}\IntegralSystem
DefaultGroupName=IntegralSystem
OutputDir=..\Output
OutputBaseFilename=IntegralSystem_%Configuration%_%PlatformName%_%Version%
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

[Registry]
Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "Carat"; ValueData: "{app}\Carat.exe"; Components: Server;

[Dirs]
Name: "{app}\Logs";
Name: "{app}\Temp";
Name: "{app}\ProcDump";

[Types]
Name: Installation; Description: "Тип установки"; Flags: iscustom;

[Components]
Name: Server; Description: "Сервер"; Types: Installation; Flags: exclusive;
Name: Client; Description: "Клиент"; Types: Installation; Flags: exclusive;

[Icons]
Name: "{userdesktop}\Carat"; Filename: "{app}\Carat.exe"; WorkingDir: "{app}"; Tasks: DesktopIconServer; Comment: "Серверный процесс Carat";
Name: "{userdesktop}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Tasks: DesktopIconClient; Comment: "Система управления данными «IntegralSystem»";
Name: "{group}\Carat"; Filename: "{app}\Carat.exe"; WorkingDir: "{app}"; Tasks: DesktopIconServer; Comment: "Серверный процесс Carat";
Name: "{group}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Comment: "Система управления данными IntegralSystem";
Name: "{group}\Конфигуратор"; Filename: "{app}\Configurator.exe"; WorkingDir: "{app}"; Tasks: DesktopIconServer; Comment: "Интерпретатор базы данных IntegralSystem";
Name: "{group}\DebugView"; Filename: "{app}\DebugView.exe"; WorkingDir: "{app}\Utility";
Name: "{group}\Документация"; Filename: "{app}\IntegralSystem.doc"; WorkingDir: "{app}"; Comment: "Документация";
Name: "{group}\Удалить Integral System"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; Comment: "Удаление Integral System";

[Tasks]
Name: "DesktopIconServer"; Description: "Создать значок «Carat» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components: Server;
Name: "DesktopIconClient"; Description: "Создать значок «IntegralSystem» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}"; Components: Server Client;

[Run]
Filename: "{app}\Carat.exe"; Description: "Запустить сервер после выхода из установщика"; Flags: postinstall shellexec skipifsilent; Components: Server;
Filename: "{app}\IntegralSystem.exe"; Description: "Запустить программу после выхода из установщика"; Flags: postinstall shellexec skipifsilent; Components: Server Client;

[INI]
Filename: {app}\Config.ini; Section: Connection; Key: Server; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Connection; Key: Port; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Connection; Key: Database; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Connection; Key: Login; String: ; Flags: createkeyifdoesntexist; Components: Server;
Filename: {app}\Config.ini; Section: Connection; Key: Password; String: ; Flags: createkeyifdoesntexist; Components: Server;
Filename: {app}\Config.ini; Section: AutoInput; Key: Included; String: false; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: AutoInput; Key: Login; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: AutoInput; Key: Password; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Other; Key: Autoboot; String: false; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Other; Key: Language; String: ru; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: DatabaseService; Key: Login; String: ; Flags: createkeyifdoesntexist; Components: Server;
Filename: {app}\Config.ini; Section: DatabaseService; Key: Password; String: ; Flags: createkeyifdoesntexist; Components: Server;
Filename: {app}\Config.ini; Section: DatabaseService; Key: FolderBackup; String: ; Flags: createkeyifdoesntexist; Components: Server;
Filename: {app}\Config.ini; Section: DatabaseService; Key: FolderPostgresBin; String: ; Flags: createkeyifdoesntexist; Components: Server;
Filename: {app}\Config.ini; Section: DatabaseService; Key: KeepOverDays; String: ; Flags: createkeyifdoesntexist; Components: Server;

[Files]
Source: ..\Components\%Configuration%-%Platform%\*; DestDir: {app}; Flags: ignoreversion;
Source: ..\Components\Utility\DebugView.exe; DestDir: {app}; Flags: ignoreversion;
Source: ..\Components\Utility\Procdump\%Platform%\*; DestDir: {app}; Flags: ignoreversion;
Source: ..\Components\VersionLib\%Platform%\*; DestDir: {app}; Flags: ignoreversion;
Source: ..\Documentation\IntegralSystem.doc; DestDir: {app}; Flags: ignoreversion;
Source: ..\Resources\Licenses\*; DestDir: {app}\Licenses; Flags: ignoreversion;

Source: ..\Deploy\%Configuration%-%Platform%\*; Excludes: "*.exp, *.ilk, *.lib, *.exe"; DestDir: {app}; Flags: ignoreversion recursesubdirs;

Source: ..\Deploy\%Configuration%-%Platform%\Carat.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\Carat.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;

Source: ..\Deploy\%Configuration%-%Platform%\CaratAsteriskQueue.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratAsteriskQueue.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratAsteriskRecord.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratAsteriskRecord.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratCalendar.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratCalendar.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratCenterSeven.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratCenterSeven.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratHighway.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratHighway.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratInformResource.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratInformResource.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratMail.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratMail.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratMedTech.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratMedTech.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratNotification.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratNotification.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratScheduler.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratScheduler.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratSMS.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratSMS.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratTelephony.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\CaratTelephony.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;

Source: ..\Deploy\%Configuration%-%Platform%\Configurator.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\Configurator.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;

Source: ..\Deploy\%Configuration%-%Platform%\DatabaseService.exe; DestDir: {app}; Flags: ignoreversion; Components: Server;
Source: ..\Deploy\%Configuration%-%Platform%\DatabaseService.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server;

Source: ..\Deploy\%Configuration%-%Platform%\EMailSender.exe; DestDir: {app}; Flags: ignoreversion; Components: Server Client;
Source: ..\Deploy\%Configuration%-%Platform%\EMailSender.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server Client;

Source: ..\Deploy\%Configuration%-%Platform%\IntegralSystem.exe; DestDir: {app}; Flags: ignoreversion; Components: Server Client;
Source: ..\Deploy\%Configuration%-%Platform%\IntegralSystem.pdb; DestDir: {app}; Flags: ignoreversion; Components: Server Client;
Source: ..\Resources\Icons\IntegralSystem.ico; DestDir: {app}; Flags: ignoreversion; Components: Server Client;
Source: ..\InstallResources\Update.cmd; DestDir: {app}; Flags: ignoreversion; Components: Server Client;

[Code]
procedure InitializeWizard();
begin
  WizardForm.WelcomeLabel2.Caption := WizardForm.WelcomeLabel2.Caption + #13#10#13#10 + 
  'Конфигурация: %Configuration%' + #13#10 +
  'Платформа: %PlatformName%' + #13#10 +
  'Версия устанавливаемой программы: %Version%';
end;