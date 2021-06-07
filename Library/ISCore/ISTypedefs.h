#pragma once
#ifndef _ISTYPEDEFS_H_INCLUDED
#define _ISTYPEDEFS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
//����������� ������
#ifdef WIN32
typedef CRITICAL_SECTION ISCriticalSection;
#else
typedef pthread_mutex_t ISCriticalSection;
#endif
//-----------------------------------------------------------------------------
//�����
#ifdef WIN32
typedef SOCKET ISSocket;
#else
typedef int ISSocket;
#endif
//-----------------------------------------------------------------------------
//������ ������
#ifdef WIN32
typedef int ISSocketLen;
#else
typedef socklen_t ISSocketLen;
#endif
//-----------------------------------------------------------------------------
//����� ������
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
#ifdef QT_USE
typedef QString ISString;
#else
typedef std::string ISString;
#endif
//-----------------------------------------------------------------------------
typedef std::vector<std::string> ISVectorString;
typedef std::vector<int> ISVectorInt;
typedef std::vector<unsigned int> ISVectorUInt;
typedef std::map<std::string, std::string> ISMapString;
typedef std::map<std::string, std::string> ISStringMapSTD;
typedef unsigned short ISTcpPort;
typedef std::chrono::time_point<std::chrono::steady_clock> ISTimePoint;

typedef long long ISInt64;
typedef unsigned long long ISUInt64;
//-----------------------------------------------------------------------------
#endif
