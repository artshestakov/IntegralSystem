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
typedef std::map<QString, QString> ISStringMap;
typedef std::map<std::string, std::string> ISStringMapSTD;
typedef std::map<QString, int> ISStringToIntMap;
typedef std::map<QString, QVariant> ISStringToVariantMap;
typedef std::vector<int> ISVectorInt;
typedef std::vector<unsigned int> ISVectorUInt;
typedef std::vector<QString> ISVectorString;
typedef std::vector<QVariant> ISVectorVariant;
typedef std::vector<QByteArray> ISVectorByteArray;
typedef std::vector<QVariantMap> ISVectorMap;
typedef std::chrono::time_point<std::chrono::steady_clock> ISTimePoint;
typedef std::pair<unsigned int, QString> ISObjectPair;
typedef long long ISInt64;
typedef unsigned long long ISUInt64;
//-----------------------------------------------------------------------------
#endif
