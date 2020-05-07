#pragma once
#ifndef _ISTCP_H_INCLUDED
#define _ISTCP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISTcp
{
public:
	static bool IsValidQuery(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString);
	static bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString);
};
//-----------------------------------------------------------------------------
#endif
