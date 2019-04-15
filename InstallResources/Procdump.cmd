SET FileDate=%DATE%

SET FileTime=%TIME:~,8%
SET FileTime=%FileTime::=.%

SET FileName=IntegralSystem_%Filedate%_%FileTime%.dmp

start %cd%\..\..\IntegralSystem.exe
Procdump.exe -t IntegralSystem.exe %cd%\..\..\ProcDump\%FileName%