#pragma once
#ifndef _ISTCP_H_INCLUDED
#define _ISTCP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISTcp
{
public:
	static bool IsValidQuery(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString); //Проверить валидность запроса
	static bool IsValidAnswer(const QByteArray &ByteArray, QVariantMap &VariantMap, QString &ErrorString); //Проверить валидность ответа на запрос
	static int GetQuerySizeFromBuffer(QByteArray &ByteArray); //Получить размер запроса из его содержимого
	static QByteArray Crypt(const std::vector<unsigned char> &Key, const QVariantMap &VariantMap); //Шифрование данных
};
//-----------------------------------------------------------------------------
#endif
