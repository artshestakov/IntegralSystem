@ECHO off

git -C . rev-list HEAD --count > Revision.tmp
SET /p REV_COUNT=<Revision.tmp
DEL Revision.tmp

ECHO #pragma once>>Revision.tmp
ECHO #ifndef _ISREVISION_H_INCLUDED>>Revision.tmp
ECHO #define _ISREVISION_H_INCLUDED>>Revision.tmp
ECHO //----------------------------------------------------------------------------->>Revision.tmp
ECHO #define CARAT_VERSION_N %REV_COUNT%>>Revision.tmp
ECHO #define CARAT_VERSION "%REV_COUNT%">>Revision.tmp
ECHO //----------------------------------------------------------------------------->>Revision.tmp
ECHO #endif>>Revision.tmp

MOVE /Y Revision.tmp ISRevision.h