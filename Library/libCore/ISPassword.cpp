#include "ISPassword.h"
#include "ISDefines.h"
#include "ISSystem.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
ISPassword::ISPassword()
{

}
//-----------------------------------------------------------------------------
ISPassword::~ISPassword()
{

}
//-----------------------------------------------------------------------------
QString ISPassword::GeneratePassword()
{
	ISUuid UID = ISSystem::GenerateUuid();
	UID.replace("{", QString());
	UID.replace("}", QString());

	QStringList StringList = UID.split("-");
	if (StringList.count() == 5)
	{
		QString GeneratedPassword = StringList[4];
		return GeneratedPassword;
	}

	return QString();
}
//-----------------------------------------------------------------------------
int ISPassword::GetWidthPassword(const QString &Password)
{
	int WeightPassword = 0;
	int PasswordLenght = 0;
	int CountFigures = 0;
	int AltSymbols = 0;
	int CountUpper = 0;

	{ //Равно 5, если количесво символов в пароле больше 5, или равно количеству символов
		int Count = Password.length();
		if (Count >= 5)
		{
			PasswordLenght = 5;
		}
		else
		{
			PasswordLenght = Count;
		}
	}

	{ //Равно 3, если количетсво цифр в пароле больше 3, иначе - равно количеству цифр
		int Count = GetCountFigures(Password);
		if (Count > 3)
		{
			CountFigures = 3;
		}
		else
		{
			CountFigures = Count;
		}
	}

	{ //Равно 3, если число символов в пароле, отличных от букв, цифр и знаков подчеркивания, больше 3, иначе - равно количеству таких символов
		int Count = GetCountAltSymbols(Password);
		if (Count > 3)
		{
			AltSymbols = 3;
		}
		else
		{
			AltSymbols = Count;
		}
	}

	{ //Равно 3, если количетсво букв в верхнем регистре больше 3, или количеству заглавных букв в противном случае
		int Count = GetCountUpperLiters(Password);
		if (Count > 3)
		{
			CountUpper = 3;
		}
		else
		{
			CountUpper = Count;
		}
	}

	WeightPassword = ((PasswordLenght * 10) - 20) + (CountFigures * 10) + (AltSymbols * 15) + (CountUpper * 10);
	return WeightPassword;
}
//-----------------------------------------------------------------------------
int ISPassword::GetCountFigures(const QString &Password)
{
	int CountFigures = 0;

	for (int i = 0; i < Password.length(); i++)
	{
		QString Symbol = Password.at(i);
		if (QString(FIGURES_STRING).contains(Symbol))
		{
			CountFigures++;
		}
	}

	return CountFigures;
}
//-----------------------------------------------------------------------------
int ISPassword::GetCountAltSymbols(const QString &Password)
{
	int AltSymbols = 0;

	QRegExp RegExp(REG_EXP_PASSWORD_ALT_SYMBOL);
	QValidator *Validator = new QRegExpValidator(RegExp);

	for (int i = 0; i < Password.length(); i++)
	{
		int x = 0;

		QString Symbol = Password.at(i);
		QValidator::State ValidatorState = Validator->validate(Symbol, x);
		if (ValidatorState == QValidator::Acceptable)
		{
			AltSymbols++;
		}
	}

	if (Validator)
	{
		delete Validator;
		Validator = nullptr;
	}

	return AltSymbols;
}
//-----------------------------------------------------------------------------
int ISPassword::GetCountUpperLiters(const QString &Password)
{
	int UpperSymbols = 0;
	for (const QChar &Symbol : Password)
	{
		int Pos = 0;
		if (QRegExpValidator(QRegExp(REG_EXP_PASSWORD_UPPER)).validate(QString(Symbol), Pos) == QValidator::Acceptable)
		{
			UpperSymbols++;
		}
	}

	return UpperSymbols;
}
//-----------------------------------------------------------------------------
