REM Если временная папка с проектом уже существует - удаляем её
IF EXIST %1 (RMDIR /S /Q %1)

REM Клонируем репозиторий и переходим в него, а затем в папку InstallWindows
git clone https://github.com/artshestakov/IntegralSystem.git %1
cd %1\InstallWindows

call _Build_IntegralSystem_Debug_Win32.cmd
call _Build_IntegralSystem_Release_Win32.cmd

REM После сборки удаляем за собой временную папку
IF EXIST %1 (RMDIR /S /Q %1)