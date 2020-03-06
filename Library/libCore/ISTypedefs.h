#pragma once
#ifndef _ISTYPEDEFS_H_INCLUDED
#define _ISTYPEDEFS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
typedef std::map<QString, QString> ISStringMap;
typedef std::map<QString, int> ISStringToIntMap;
typedef std::vector<int> ISVectorInt;
typedef std::vector<QString> ISVectorString;
typedef std::vector<ISUuid> ISVectorUID;
typedef std::vector<QVariant> ISVectorVariant;
//-----------------------------------------------------------------------------
#endif
