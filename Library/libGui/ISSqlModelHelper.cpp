#include "ISSqlModelHelper.h"
#include "ISConstants.h"
#include "ISPhoneNumberParser.h"
#include "ISSettings.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
QVariant ISSqlModelHelper::ValueForType(const QVariant &Value, ISNamespace::FieldType Type)
{
	QVariant Result = Value;
	if (Type == ISNamespace::FT_Date)
	{
		Result = Value.toDate().toString(SETTING_STRING(CONST_UID_SETTING_TABLE_DATE_FORMAT));
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
		Result = ISGui::ConvertDateTimeToString(Value.toDateTime(), SETTING_STRING(CONST_UID_SETTING_TABLE_DATE_FORMAT), SETTING_STRING(CONST_UID_SETTING_TABLE_TIME_FORMAT));
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
	else if (Type == ISNamespace::FT_Double)
	{
		Result.setValue<QString>(QString::number(Value.toDouble(), 'g', SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA) + 1));
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
