#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISPhoneNumberParser
{
public:
	static QString RemoveNotDigits(const QString &String); //Удаление НЕ цифр из строки
	static QString PasteEvent(const QString &PastedString); //Парсинг вставляемой строки
};
//-----------------------------------------------------------------------------
