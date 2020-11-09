#pragma once
#ifndef _ISTYPEDEFS_H_INCLUDED
#define _ISTYPEDEFS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
typedef CRITICAL_SECTION ISCriticalSection;
#else
typedef pthread_mutex_t ISCriticalSection;
#endif
typedef std::map<QString, QString> ISStringMap;
typedef std::map<QString, int> ISStringToIntMap;
typedef std::map<QString, QVariant> ISStringToVariantMap;
typedef std::vector<int> ISVectorInt;
typedef std::vector<QString> ISVectorString;
typedef std::vector<ISUuid> ISVectorUID;
typedef std::vector<QVariant> ISVectorVariant;
typedef std::vector<QByteArray> ISVectorByteArray;
typedef std::vector<QVariantMap> ISVectorMap;
typedef std::vector<std::pair<QString, QString>> ISAmiPackage;
typedef std::chrono::time_point<std::chrono::steady_clock> ISTimePoint;
//-----------------------------------------------------------------------------
#endif
