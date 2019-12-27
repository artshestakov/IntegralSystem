#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISPhoneNumberParser
{
public:
	static QString RemoveNotDigits(const QString &String); //Удаление НЕ цифр из строки
	static QString PasteEvent(const QString &PastedString); //Парсинг вставляемой строки
};
//-----------------------------------------------------------------------------
