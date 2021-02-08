[Setup]
AppId={{757D746C-0676-404E-BC4C-40246AB5E38}
AppName=IntegralSystem
AppVerName=IntegralSystem
AppVersion=${VERSION}
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
Compression=lzma2/ultra64
SolidCompression=yes
UninstallDisplayIcon={app}\IntegralSystem.ico
UninstallDisplayName=IntegralSystem
DisableWelcomePage=no

[Languages]
Name: "Russian"; MessagesFile: "compiler:Languages\Russian.isl";

[Tasks]
Name: "IconDesktop"; Description: "Создать значок «IntegralSystem» на рабочем столе"; GroupDescription: "{cm:AdditionalIcons}";

[Icons]
Name: "{commondesktop}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Tasks: IconDesktop; Comment: "Система управления данными «IntegralSystem»";
Name: "{group}\Integral System"; Filename: "{app}\IntegralSystem.exe"; WorkingDir: "{app}"; Comment: "Система управления данными IntegralSystem";
Name: "{group}\Удалить Integral System"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; Comment: "Удаление Integral System";

[Run]
Filename: "{app}\IntegralSystem.exe"; Description: "Запустить программу после выхода из установщика"; Flags: postinstall shellexec skipifsilent;

[Files]
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\ISCore.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\ISGui.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Deploy\${CONFIGURATION}-${PLATFORM}\IntegralSystem.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;

Source: ${QTDIR}\bin\libEGL${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\libGLESV2${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Core${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Gui${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Multimedia${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Network${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Widgets${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ${QTDIR}\bin\Qt5Xml${IS_DEBUG}.dll; DestDir: {app}; Flags: ignoreversion recursesubdirs;

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
Source: ${QTDIR}\plugins\styles\qwindowsvistastyle${IS_DEBUG}.dll; DestDir: {app}\styles; Flags: ignoreversion recursesubdirs;

Source: ..\Resources\Icons\IntegralSystem.ico; DestDir: {app}; Flags: ignoreversion recursesubdirs;
Source: ..\Components\Redistributable\Redistributable_2015_${PLATFORM}.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs;

[Run]
Filename: {app}\Redistributable_2015_${PLATFORM}.exe; Description: "Установка VC++ Redistributable 2015 ${PLATFORM}"; Parameters: "/install /quiet"; WorkingDir: {app}; StatusMsg: "Установка VC++ Redistributable 2015 ${PLATFORM}...";

[Code]
var
  IsAutoStart: Boolean; // Флаг автоматиеского запуска программы
  
procedure InitializeWizard();
var
  i: Integer;
  
begin
  WizardForm.WelcomeLabel2.Caption := WizardForm.WelcomeLabel2.Caption + #13#10#13#10 + 
  'Конфигурация: ${CONFIGURATION}' + #13#10 +
  'Платформа: ${PLATFORM}' + #13#10 +
  'Версия устанавливаемой программы: ${VERSION}';
  
  //Проверяем наличие агумента "/silent", если есть - выставляем соотв. переменную в True
  IsAutoStart := False;
  for i := 1 to ParamCount do
    if CompareText(ParamStr(i), '/silent') = 0 then
    begin
      IsAutoStart := True;
      Break;
    end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  
begin
  if CurStep = ssPostInstall then //Если текущий шаг - послеустановочный
  begin
    if IsAutoStart then //Если необходим автоматический запуск
      if not Exec(ExpandConstant('{app}\IntegralSystem.exe'), '', '', SW_SHOW, ewNoWait, ResultCode) then
        begin
            MsgBox('Ошибка автоматического запуска программы: ' + SysErrorMessage(ResultCode), mbError, MB_OK);
        end;
  end;
end;