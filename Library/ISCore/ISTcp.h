#pragma once
#ifndef _ISTCP_H_INCLUDED
#define _ISTCP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISTcp
{
public:
	static bool IsValidQuery(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString); //ѕроверить валидность запроса
	static bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString); //ѕроверить валидность ответа на запрос
	static long GetPacketSizeFromBuffer(QByteArray &ByteArray); //ѕолучить размер пакета из его содержимого
};
//-----------------------------------------------------------------------------
#endif
