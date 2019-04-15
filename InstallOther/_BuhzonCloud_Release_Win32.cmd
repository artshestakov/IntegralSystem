@ECHO off

call Scripts\ChangeQt.cmd
call Scripts\vcvars32.bat
call WinDeployClear.cmd

IF EXIST ..\Projects\BuhzonCloud\Resources\Version.txt DEL ..\Projects\BuhzonCloud\Resources\Version.txt
IF EXIST ..\Projects\BuhzonCloud\Revision.txt DEL ..\Projects\BuhzonCloud\Revision.txt

git rev-list HEAD --count >> ..\Projects\BuhzonCloud\Revision.txt

SET /p MAJOR=<..\Projects\BuhzonCloud\Major.txt
SET /p MINOR=<..\Projects\BuhzonCloud\Minor.txt
SET /p REVISION=<..\Projects\BuhzonCloud\Revision.txt

ECHO %MAJOR%.%MINOR%.%REVISION% >> ..\Projects\BuhzonCloud\Resources\Version.txt

MSBuild.exe BuhzonCloud.proj