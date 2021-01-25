#include "ISTcpWorkerHelper.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISAlgorithm.h"
#include "ISProperty.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::ConvertDateTimeToString(const QDateTime &DateTime, const QString &TimeFormat)
{
	return DateTime.isValid() ? ConvertDateToString(DateTime.date()) + SYMBOL_SPACE +
		LANG("Carat.In") + SYMBOL_SPACE + DateTime.time().toString(TimeFormat) :
		QString();
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::ConvertDateToString(const QDate &Date)
{
	QString Result;
	QDate CurrentDate = QDate::currentDate();
	if (Date == CurrentDate.addDays(-2)) //Позавчера
	{
		Result = LANG("Carat.BeforeYesterday");
	}
	else if (Date == CurrentDate.addDays(-1)) //Вчера
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == CurrentDate) //Сегодня
	{
		Result = LANG("Carat.Today");
	}
	else if (Date == CurrentDate.addDays(1)) //Завтра
	{
		Result = LANG("Carat.Tomorrow");
	}
	else if (Date == CurrentDate.addDays(2)) //Послезавтра
	{
		Result = LANG("Carat.AfterTomorrow");
	}
	else
	{
		switch (Date.month())
		{
		case 1: Result = LANG("Carat.Month.January"); break;
		case 2: Result = LANG("Carat.Month.February"); break;
		case 3: Result = LANG("Carat.Month.March"); break;
		case 4: Result = LANG("Carat.Month.April"); break;
		case 5: Result = LANG("Carat.Month.May"); break;
		case 6: Result = LANG("Carat.Month.June"); break;
		case 7: Result = LANG("Carat.Month.July"); break;
		case 8: Result = LANG("Carat.Month.August"); break;
		case 9: Result = LANG("Carat.Month.September"); break;
		case 10: Result = LANG("Carat.Month.October"); break;
		case 11: Result = LANG("Carat.Month.November"); break;
		case 12: Result = LANG("Carat.Month.December"); break;
		default:
			break;
		}
		Result = QString::number(Date.day()) + SYMBOL_SPACE +
			Result + SYMBOL_SPACE +
			QString::number(Date.year());
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::GetColorForLogMessage(const QString &Severity)
{
	if (Severity == LOGGER_SEVERITY_DEBUG)
	{
		return ISAlgorithm::RGBToHEX(0, 0, 255); //Синий
	}
	else if (Severity == LOGGER_SEVERITY_INFO)
	{
		return ISAlgorithm::RGBToHEX(0, 128, 0); //Тёмно-зелёный
	}
	else if (Severity == LOGGER_SEVERITY_WARNING)
	{
		return ISAlgorithm::RGBToHEX(226, 132, 0); //Оранжевый
	}
	else if (Severity == LOGGER_SEVERITY_ERROR || Severity == LOGGER_SEVERITY_CRITICAL)
	{
		return ISAlgorithm::RGBToHEX(255, 0, 0); //Красный
	}
	else if (Severity == LOGGER_SEVERITY_TRACE || Severity == LOGGER_SEVERITY_ASSERT)
	{
		return ISAlgorithm::RGBToHEX(128, 0, 128); //Фиолетовый
	}
	return ISAlgorithm::RGBToHEX(0, 0, 0); //Чёрный
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::GetUptime()
{
	qint64 Seconds = PROPERTY_GET("Uptime").toDateTime().secsTo(QDateTime::currentDateTime()),
		Days = 0;
	while (Seconds > DAY_IN_SECONDS)
	{
		++Days;
		Seconds -= DAY_IN_SECONDS;
	}
	return LANG("Carat.Uptime").arg(Days).arg(QTime(0, 0).addSecs(Seconds).toString(FORMAT_TIME_V3));
}
//-----------------------------------------------------------------------------
QVariant ISTcpWorkerHelper::GetSettingDB(const QString &DBConnectionName, const QString &SettingName)
{
	QVariant Value;
	ISQuery qSelectSettingDB(ISDatabase::Instance().GetDB(DBConnectionName),
		"SELECT sgdb_" + SettingName + " FROM _settingsdatabase WHERE sgdb_uid = :SettingUID");
	qSelectSettingDB.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectSettingDB.Execute()) //Запрос выполнен успешно
	{
		if (qSelectSettingDB.First()) //Настройка нашлась
		{
			Value = qSelectSettingDB.ReadColumn(0);
		}
		else //Такой настройки в БД нет
		{
			ISLOGGER_E(__CLASS__, "not found setting database. Use default value...");
		}
	}
	else //Ошибка запроса
	{
		ISLOGGER_E(__CLASS__, "not getting setting database: " + qSelectSettingDB.GetErrorString());
	}

	//Если значение невалидное - используем указанное по умолчанию
	if (!Value.isValid() || Value.isNull())
	{
		Value = ISMetaData::Instance().GetMetaTable("_SettingsDatabase")->GetField(SettingName)->DefaultValue;
	}
	return Value;
}
//-----------------------------------------------------------------------------
QString ISTcpWorkerHelper::GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);
	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignViewNameField + "\n" +
		"FROM " + MetaTableForeign->Name + "\n" +
		"WHERE " + MetaTableForeign->Alias + "_id = " + Alias + '_' + FieldName;
	return SqlQuery;
}
//-----------------------------------------------------------------------------
