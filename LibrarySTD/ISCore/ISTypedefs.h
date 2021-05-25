#pragma once
#ifndef _ISTYPEDEFS_H_INCLUDED
#define _ISTYPEDEFS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
//Критическая секция
#ifdef WIN32
typedef CRITICAL_SECTION ISCriticalSection;
#else
typedef pthread_mutex_t ISCriticalSection;
#endif
//-----------------------------------------------------------------------------
//Сокет
#ifdef WIN32
typedef SOCKET ISSocket;
#else
typedef int ISSocket;
#endif
//-----------------------------------------------------------------------------
//Размер сокета
#ifdef WIN32
typedef int ISSocketLen;
#else
typedef socklen_t ISSocketLen;
#endif
//-----------------------------------------------------------------------------
//Номер ошибки
#ifdef WIN32
typedef DWORD ISErrorNumber;
#else
typedef int ISErrorNumber;
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
typedef SOCKADDR_IN ISSocketAddr;
#else
typedef sockaddr_in ISSocketAddr;
#endif
//-----------------------------------------------------------------------------
typedef std::vector<std::string> ISVectorString;
typedef std::map<std::string, std::string> ISMapString;
typedef unsigned short ISTcpPort;
typedef std::chrono::time_point<std::chrono::steady_clock> ISTimePoint;
//-----------------------------------------------------------------------------
#endif
