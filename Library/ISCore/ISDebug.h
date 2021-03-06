#pragma once
#ifndef _ISDEBUG_H_INCLUDED
#define _ISDEBUG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ShowDebugString(const char *Severity, const std::string &String);
void ShowDebugString(const std::string &String);
//-----------------------------------------------------------------------------
#define ISDEBUG std::cout << std::endl
#define ISDEBUG_L(MESSAGE) ShowDebugString(MESSAGE)
#define ISDEBUG_D(MESSAGE) ShowDebugString("Debug", MESSAGE)
#define ISDEBUG_I(MESSAGE) ShowDebugString("Info", MESSAGE)
#define ISDEBUG_W(MESSAGE) ShowDebugString("Warning", MESSAGE)
#define ISDEBUG_E(MESSAGE) ShowDebugString("Error", MESSAGE)
//-----------------------------------------------------------------------------
#endif
