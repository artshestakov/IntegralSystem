#include "ISPhoneNumberParser.h"
//-----------------------------------------------------------------------------
QString ISPhoneNumberParser::RemoveNotDigits(const QString &String)
{
	QString ResultString;
	for (int i = 0; i < String.count(); ++i)
	{
		QChar Symbol = String.at(i);
		if (Symbol.isDigit())
		{
			ResultString.append(Symbol);
		}
	}
	return ResultString;
}
//-----------------------------------------------------------------------------
QString ISPhoneNumberParser::PasteEvent(const QString &PastedString)
{
	QString Temp = RemoveNotDigits(PastedString);
	QString ResultString;
	for (int i = Temp.count() - 1; i >= 0; i--)
	{
		if (Temp[i].isDigit())
		{
			ResultString.insert(0, Temp[i]);
		}

		if (ResultString.length() == 10)
		{
			break;
		}
	}
	return ResultString;
}
//-----------------------------------------------------------------------------
