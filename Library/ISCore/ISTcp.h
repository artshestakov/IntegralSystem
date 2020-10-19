#pragma once
#ifndef _ISTCP_H_INCLUDED
#define _ISTCP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcp
{
public:
	static bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString); //ѕроверить валидность ответа на запрос
	static int GetQuerySizeFromBuffer(QByteArray &ByteArray, bool &Ok); //ѕолучить размер запроса из его содержимого
	static void WaitForBytesWritten(QTcpSocket *TcpSocket); //ќжидание записи данных в сокет
};
//-----------------------------------------------------------------------------
#endif
