#include "StdAfx.h"
#include "ISSqlModelHelper.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISPhoneNumberParser.h"
#include "ISSettings.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
ISSqlModelHelper::ISSqlModelHelper() : QObject()
{

}
//-----------------------------------------------------------------------------
ISSqlModelHelper::~ISSqlModelHelper()
{

}
//-----------------------------------------------------------------------------
QVariant ISSqlModelHelper::ValueForType(const QVariant &Value, ISNamespace::FieldType Type)
{
	QVariant Result = Value;

	if (Type == ISNamespace::FT_Date)
	{
		QString DateFormat = SETTING_STRING(CONST_UID_SETTING_TABLE_DATE_FORMAT);
		if (DateFormat == "Default")
		{
			Result = Value.toDate().toString(DATE_FORMAT_V1);
		}
		else if (DateFormat == "Extended")
		{
			Result = ISCore::ConvertDateToString(Value.toDate(), DATE_FORMAT_V1);
		}
	}
	else if (Type == ISNamespace::FT_Birthday)
	{
		Result = Value.toDate().toString(DATE_FORMAT_V1);
	}
	else if (Type == ISNamespace::FT_Time)
	{
		Result = Value.toTime().toString(TIME_FORMAT_V3);
	}
	else if (Type == ISNamespace::FT_DateTime)
	{
		QString DateFormat = SETTING_STRING(CONST_UID_SETTING_TABLE_DATE_FORMAT);
		if (DateFormat == "Default")
		{
			Result = Value.toDateTime().toString(DATE_TIME_FORMAT_V1);
		}
		else if (DateFormat == "Extended")
		{
			Result = ISCore::ConvertDateTimeToString(Value.toDateTime(), DATE_FORMAT_V1, TIME_FORMAT_V1);
		}
	}
	else if (Type == ISNamespace::FT_Password)
	{
		QString ValueString = Result.toString();
		for (int i = 0; i < ValueString.count(); i++)
		{
			ValueString.replace(i, 1, SYMBOL_CIRCLE);
		}
		Result = ValueString;
	}
	else if (Type == ISNamespace::FT_Phone || Type == ISNamespace::FT_PhoneBase)
	{
		Result = "+7" + Result.toString();
	}
	else if (Type == ISNamespace::FT_Seconds)
	{
		Result = QTime(0, 0).addSecs(Value.toInt()).toString(TIME_FORMAT_V3);
	}
	else if (Type == ISNamespace::FT_Percent)
	{
		Result = Value.toString() + "%";
	}

	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISSqlModelHelper::ValueFromTextAlignment(ISNamespace::FieldType FieldType, PMetaClassForeign *MetaForeign)
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
