#pragma once
#ifndef _ISTYPEDEFS_H_INCLUDED
#define _ISTYPEDEFS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
typedef CRITICAL_SECTION ISCriticalSection;
#else
typedef pthread_mutex_t ISCriticalSection;
#endif
typedef std::vector<std::string> ISVectorString;
typedef long long ISInt64;
typedef unsigned long long ISUInt64;
//-----------------------------------------------------------------------------
#endif
