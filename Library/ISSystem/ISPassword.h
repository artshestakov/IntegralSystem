#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISPassword
{
public:
	ISPassword();
	virtual ~ISPassword();

	static QString GeneratePassword(); //Сгенерировать пароль
	static int GetWidthPassword(const QString &Password); //Получить "вес" пароля

protected:
	static int GetCountFigures(const QString &Password); //Получить количетсво цифр в пароле
	static int GetCountAltSymbols(const QString &Password); //Получить количество альтернативных символов
	static int GetCountUpperLiters(const QString &Password); //Получить количество букв в верхнем регистре
};
//-----------------------------------------------------------------------------
