@ECHO off

SET SOLTION_DIR=%1

git -C . rev-list HEAD --count > %SOLTION_DIR%Revision.tmp
SET /p REV_COUNT=<%SOLTION_DIR%Revision.tmp
DEL %SOLTION_DIR%Revision.tmp

ECHO #pragma once>>%SOLTION_DIR%Revision.tmp
ECHO #ifndef _ISREVISION_H_INCLUDED>>%SOLTION_DIR%Revision.tmp
ECHO #define _ISREVISION_H_INCLUDED>>%SOLTION_DIR%Revision.tmp
ECHO //----------------------------------------------------------------------------->>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_VERSION_N %REV_COUNT%>>%SOLTION_DIR%Revision.tmp
ECHO #define CARAT_VERSION "%REV_COUNT%">>%SOLTION_DIR%Revision.tmp
ECHO //----------------------------------------------------------------------------->>%SOLTION_DIR%Revision.tmp
ECHO #endif>>%SOLTION_DIR%Revision.tmp

MOVE /Y %SOLTION_DIR%Revision.tmp %SOLTION_DIR%ISRevision.h