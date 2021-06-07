#pragma once
#ifndef _ISTYPEDEFSGUI_H_INCLUDED
#define _ISTYPEDEFSGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
typedef std::map<QString, QString> ISStringMapQt;
typedef std::map<QString, int> ISStringToIntMapQt;
typedef std::map<QString, QVariant> ISStringToVariantMapQt;
typedef std::vector<QString> ISVectorStringQt;
typedef std::vector<QVariant> ISVectorVariantQt;
typedef std::vector<QByteArray> ISVectorByteArrayQt;
typedef std::vector<QVariantMap> ISVectorMapQt;
typedef std::pair<unsigned int, QString> ISObjectPair;
//-----------------------------------------------------------------------------
#endif
