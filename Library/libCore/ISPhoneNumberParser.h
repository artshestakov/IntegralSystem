#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISPhoneNumberParser
{
public:
	static QString RemoveNotDigits(const QString &String); //Удаление НЕ цифр из строки
	static QString PasteEvent(const QString &PastedString); //Парсинг вставляемой строки
};
//-----------------------------------------------------------------------------
