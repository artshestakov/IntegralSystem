@ECHO off

SET SOLTION_DIR=%1
SET CONFIGURATION=%2
SET PLATFORM=%3

git -C . rev-list HEAD --count > %SOLTION_DIR%Revision.tmp
SET /p REVISION=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

ECHO #pragma once>>%SOLTION_DIR%Revision.tmp
ECHO #ifndef _ISREVISION_H_INCLUDED>>%SOLTION_DIR%Revision.tmp
ECHO #define _ISREVISION_H_INCLUDED>>%SOLTION_DIR%Revision.tmp
ECHO //----------------------------------------------------------------------------->>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_VERSION_N %REVISION%>>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_VERSION "%REVISION%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_CONFIGURATION "%CONFIGURATION%">>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_PLATFORM "%PLATFORM%">>%SOLTION_DIR%Revision.tmp
ECHO //----------------------------------------------------------------------------->>%SOLTION_DIR%Revision.tmp
ECHO #endif>>%SOLTION_DIR%Revision.tmp

MOVE /Y %SOLTION_DIR%Revision.tmp %SOLTION_DIR%ISRevision.h