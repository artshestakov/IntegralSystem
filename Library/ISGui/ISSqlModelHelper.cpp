#include "ISSqlModelHelper.h"
#include "ISConstants.h"
#include "ISPhoneNumberParser.h"
#include "ISSettings.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISSettingsDatabase.h"
#include "ISAlgorithm.h"
#include "ISValidators.h"
//-----------------------------------------------------------------------------
QVariant ISSqlModelHelper::ValueForType(const QVariant &Value, ISNamespace::FieldType Type)
{
	QVariant Result = Value;
	if (Type == ISNamespace::FT_Date)
	{
		Result = Value.toDate().toString(SETTING_STRING(CONST_UID_SETTING_OTHER_DATE_FORMAT));
	}
	else if (Type == ISNamespace::FT_Birthday)
	{
		Result = Value.toDate().toString(FORMAT_DATE_V1);
	}
	else if (Type == ISNamespace::FT_Time)
	{
		Result = Value.toTime().toString(FORMAT_TIME_V3);
	}
	else if (Type == ISNamespace::FT_DateTime)
	{
		Result = ISGui::ConvertDateTimeToString(Value.toDateTime(), SETTING_STRING(CONST_UID_SETTING_OTHER_DATE_FORMAT), SETTING_STRING(CONST_UID_SETTING_OTHER_TIME_FORMAT));
	}
	else if (Type == ISNamespace::FT_Password)
	{
		QString ValueString = Result.toString();
		for (int i = 0; i < ValueString.count(); ++i)
		{
			ValueString.replace(i, 1, ISDefines::Core::SYMBOL_CIRCLE);
		}
		Result = ValueString;
	}
	else if (Type == ISNamespace::FT_Phone)
	{
		Result = "+7" + Result.toString();
	}
	else if (Type == ISNamespace::FT_Seconds)
	{
		Result = QTime(0, 0).addSecs(Value.toInt()).toString(FORMAT_TIME_V3);
	}
	else if (Type == ISNamespace::FT_Double)
	{
		QString String = Value.toString();
		int Pos = 0;
		ISDoubleValidator(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA)).validate(String, Pos);
		Result = String;
	}
	else if (Type == ISNamespace::FT_UID)
	{
		Result = ISUuid(Result);
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelHelper::ValueFromTextAlignment(ISNamespace::FieldType FieldType, PMetaForeign *MetaForeign)
{
	QVariant Result;
	if (FieldType == ISNamespace::FT_Date ||
		FieldType == ISNamespace::FT_Time ||
		FieldType == ISNamespace::FT_DateTime ||
		FieldType == ISNamespace::FT_Birthday ||
		FieldType == ISNamespace::FT_Seconds ||
		FieldType == ISNamespace::FT_Year ||
		FieldType == ISNamespace::FT_Phone)
	{
		Result = Qt::AlignCenter;
	}
	else if (FieldType == ISNamespace::FT_Int || FieldType == ISNamespace::FT_Double)
	{
		if (!MetaForeign)
		{
			Result = Qt::AlignRight + Qt::AlignVCenter;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
