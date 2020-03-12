#include "ISSqlModelHelper.h"
#include "ISConstants.h"
#include "ISPhoneNumberParser.h"
#include "ISSettings.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
QVariant ISSqlModelHelper::ValueForType(const QVariant &Value, ISNamespace::FieldType Type)
{
	QVariant Result = Value;

	if (Type == ISNamespace::FT_Date)
	{
		QString DateFormat = SETTING_STRING(CONST_UID_SETTING_TABLE_DATE_FORMAT);
		if (DateFormat == "Default")
		{
			Result = Value.toDate().toString(FORMAT_DATE_V1);
		}
		else if (DateFormat == "Extended")
		{
			Result = ISGui::ConvertDateToString(Value.toDate(), FORMAT_DATE_V1);
		}
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
		QString DateFormat = SETTING_STRING(CONST_UID_SETTING_TABLE_DATE_FORMAT);
		if (DateFormat == "Default")
		{
			Result = Value.toDateTime().toString(FORMAT_DATE_TIME_V1);
		}
		else if (DateFormat == "Extended")
		{
			Result = ISGui::ConvertDateTimeToString(Value.toDateTime(), FORMAT_DATE_V1, FORMAT_TIME_V1);
		}
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
	else if (Type == ISNamespace::FT_Phone || Type == ISNamespace::FT_PhoneBase)
	{
		Result = "+7" + Result.toString();
	}
	else if (Type == ISNamespace::FT_Seconds)
	{
		Result = QTime(0, 0).addSecs(Value.toInt()).toString(FORMAT_TIME_V3);
	}
	else if (Type == ISNamespace::FT_Percent)
	{
		Result = Value.toString() + '%';
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
		FieldType == ISNamespace::FT_Phone ||
		FieldType == ISNamespace::FT_PhoneBase ||
		FieldType == ISNamespace::FT_Percent)
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
QString ISSqlModelHelper::ValueForToolTip(bool ShowToolTip, const QVariant &Value, ISNamespace::FieldType FieldType)
{
	QString Result = Value.toString();

	if (ShowToolTip)
	{
		if (FieldType == ISNamespace::FT_Bool || FieldType == ISNamespace::FT_ByteArray || FieldType == ISNamespace::FT_CallDetails)
		{
			return QString();
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
