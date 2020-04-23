#include "ISPhoneNumberParser.h"
//-----------------------------------------------------------------------------
QString ISPhoneNumberParser::RemoveNotDigits(const QString &String)
{
	QString ResultString;
	for (const QChar &Char : String)
	{
		if (Char.isDigit())
		{
			ResultString.append(Char);
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
