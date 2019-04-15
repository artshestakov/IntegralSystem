[Setup]
AppId={{E7FBC9C5-171B-4F3E-A9BB-F7F1730AB95D}
AppName=Buhzon Cloud
AppVerName=Buhzon Cloud
AppVersion=1.0.0.0
VersionInfoVersion=1.0.0.0
DefaultDirName={sd}\Buhzon Cloud
DefaultGroupName=Buhzon Cloud
OutputDir=..\Output
OutputBaseFilename=Buhzon Cloud
AlwaysShowDirOnReadyPage=yes
ShowLanguageDialog=yes
AllowRootDirectory=yes
DisableProgramGroupPage=no
AppPublisher=Shestakov Artem
CreateUninstallRegKey=yes
DirExistsWarning=auto
WizardImageStretch=yes
SetupIconFile=..\Projects\BuhzonCloud\Icon.ico
FlatComponentsList=no
ShowComponentSizes=yes
Compression=lzma2/ultra64
SolidCompression=yes
UninstallDisplayIcon={app}\Icon.ico
UninstallDisplayName=Buhzon Cloud
DisableWelcomePage=no

[Languages]
Name: "Russian"; MessagesFile: "compiler:Languages\Russian.isl";

[Dirs]
Name: "{app}\Temp";

[Icons]
Name: "{userdesktop}\Buhzon Cloud"; Filename: "{app}\BuhzonCloud.exe"; WorkingDir: "{app}";
Name: "{group}\Buhzon Cloud"; Filename: "{app}\BuhzonCloud.exe"; WorkingDir: "{app}";
Name: "{group}\Удалить Buhzon Cloud"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; Comment: "Удалить Buhzon Cloud";

[Run]
Filename: "{app}\BuhzonCloud.exe"; Description: "Запустить облачный сервис после выхода из установщика"; Flags: postinstall shellexec skipifsilent;

[INI]
Filename: {app}\Config.ini; Section: Connection; Key: Login; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Connection; Key: Password; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Connection; Key: ConnectionType; String: ; Flags: createkeyifdoesntexist;
Filename: {app}\Config.ini; Section: Other; Key: HideContent; String: false; Flags: createkeyifdoesntexist;

[Files]
Source: ..\Projects\BuhzonCloud\Icon.ico; DestDir: {app}; Flags: ignoreversion;
Source: ..\Projects\BuhzonCloud\CryptPasswordRDP.exe; DestDir: {app}; Flags: ignoreversion;
Source: ..\Deploy\Release-Win32\*; Excludes: "*.exp, *.ilk, *.lib"; DestDir: {app}; Flags: ignoreversion recursesubdirs;