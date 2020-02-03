#include "ISRandom.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
int ISRandom::Integer()
{
	return QTime::currentTime().msec();
}
//-----------------------------------------------------------------------------
int ISRandom::Integer(int Minimum, int Maximum)
{
	int Result = 0;
	while (true)
	{
		QString TempString = QString::number(QTime::currentTime().msec());
		if (TempString.length() > 0)
		{
			TempString.remove(0, TempString.length() - 1);
		}

		int TempInt = TempString.toInt();
		if (TempInt >= Minimum && TempInt <= Maximum)
		{
			Result = TempInt;
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
double ISRandom::Double()
{
	return (double)Integer() + (1 / (double)Integer(1, 10));
}
//-----------------------------------------------------------------------------
QString ISRandom::String(int Symbols)
{
	QString Result;
	if (Symbols)
	{
		QString Code = ISSystem::GenerateUuid();
		Code.replace('{', QString());
		Code.replace('}', QString());
		Code.replace('-', QString());
		if (Symbols > Code.length())
		{
			return Result;
		}

		for (int i = 0; i < Symbols; ++i)
		{
			Result += Code.at(i);
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
