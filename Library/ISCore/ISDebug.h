#pragma once
#ifndef _ISDEBUG_H_INCLUDED
#define _ISDEBUG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCORE_EXPORT void ShowDebugString(const char *Severity, const QString &String);
ISCORE_EXPORT void ShowDebugString(const QString &String);
//-----------------------------------------------------------------------------
#define ISDEBUG() std::cout << std::endl;
#define ISDEBUG_L(MESSAGE) ShowDebugString(MESSAGE)
#define ISDEBUG_D(MESSAGE) ShowDebugString(LOGGER_SEVERITY_DEBUG, MESSAGE)
#define ISDEBUG_I(MESSAGE) ShowDebugString(LOGGER_SEVERITY_INFO, MESSAGE)
#define ISDEBUG_W(MESSAGE) ShowDebugString(LOGGER_SEVERITY_WARNING, MESSAGE)
#define ISDEBUG_E(MESSAGE) ShowDebugString(LOGGER_SEVERITY_ERROR, MESSAGE)
//-----------------------------------------------------------------------------
#endif
