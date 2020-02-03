#include "ISPassword.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
QString ISPassword::GeneratePassword()
{
	ISUuid UID = ISSystem::GenerateUuid();
	UID.replace('{', QString());
	UID.replace('}', QString());

	QStringList StringList = UID.split(SYMBOL_MINUS);
	if (StringList.count() == 5)
	{
		return StringList[4];
	}
	return QString();
}
//-----------------------------------------------------------------------------
int ISPassword::GetWidthPassword(const QString &Password)
{
	int WeightPassword = 0, PasswordLenght = 0, CountFigures = 0, AltSymbols = 0, CountUpper = 0;

	{ //Равно 5, если количесво символов в пароле больше 5, или равно количеству символов
		int Count = Password.length();
		Count >= 5 ? PasswordLenght = 5 : PasswordLenght = Count;
	}

	{ //Равно 3, если количетсво цифр в пароле больше 3, иначе - равно количеству цифр
		int Count = GetCountFigures(Password);
		Count > 3 ? CountFigures = 3 : CountFigures = Count;
	}

	{ //Равно 3, если число символов в пароле, отличных от букв, цифр и знаков подчеркивания, больше 3, иначе - равно количеству таких символов
		int Count = GetCountAltSymbols(Password);
		Count > 3 ? AltSymbols = 3 : AltSymbols = Count;
	}

	{ //Равно 3, если количетсво букв в верхнем регистре больше 3, или количеству заглавных букв в противном случае
		int Count = GetCountUpperLiters(Password);
		Count > 3 ? CountUpper = 3 : CountUpper = Count;
	}

	return ((PasswordLenght * 10) - 20) + (CountFigures * 10) + (AltSymbols * 15) + (CountUpper * 10);
}
//-----------------------------------------------------------------------------
int ISPassword::GetCountFigures(const QString &Password)
{
	int CountFigures = 0;
	for (const QChar &Symbol : Password)
	{
		if (QString(FIGURES_STRING).contains(Symbol))
		{
			++CountFigures;
		}
	}
	return CountFigures;
}
//-----------------------------------------------------------------------------
int ISPassword::GetCountAltSymbols(const QString &Password)
{
	int AltSymbols = 0;
	for (const QChar &Symbol : Password)
	{
		int Position = 0;
		if (QRegExpValidator(QRegExp(REG_EXP_PASSWORD_ALT_SYMBOL)).validate(QString(Symbol), Position) == QRegExpValidator::Acceptable)
		{
			++AltSymbols;
		}
	}
	return AltSymbols;
}
//-----------------------------------------------------------------------------
int ISPassword::GetCountUpperLiters(const QString &Password)
{
	int UpperSymbols = 0;
	for (const QChar &Symbol : Password)
	{
		int Position = 0;
		if (QRegExpValidator(QRegExp(REG_EXP_PASSWORD_UPPER)).validate(QString(Symbol), Position) == QRegExpValidator::Acceptable)
		{
			++UpperSymbols;
		}
	}

	return UpperSymbols;
}
//-----------------------------------------------------------------------------
