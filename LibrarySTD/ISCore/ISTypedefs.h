#pragma once
#ifndef _ISTYPEDEFS_H_INCLUDED
#define _ISTYPEDEFS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
typedef CRITICAL_SECTION ISCriticalSection;
typedef SOCKET ISSocket;
typedef SOCKADDR_IN ISSocketAddr;
typedef int ISSocketLen;
typedef DWORD ISErrorNumber;
#else
typedef pthread_mutex_t ISCriticalSection;
typedef int ISSocket;
typedef sockaddr_in ISSocketAddr;
typedef socklen_t ISSocketLen;
typedef int ISErrorNumber;
#endif
typedef std::vector<std::string> ISVectorString;
typedef std::map<std::string, std::string> ISMapString;
typedef long long ISInt64;
typedef unsigned long long ISUInt64;
typedef unsigned short ISTcpPort;
typedef std::chrono::time_point<std::chrono::steady_clock> ISTimePoint;
//-----------------------------------------------------------------------------
#endif
