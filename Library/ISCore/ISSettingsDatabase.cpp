#include "ISSettingsDatabase.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISQueryModel.h"
//-----------------------------------------------------------------------------
ISSettingsDatabase::ISSettingsDatabase()
	: ErrorString(NO_ERROR_STRING)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISSettingsDatabase::~ISSettingsDatabase()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISSettingsDatabase& ISSettingsDatabase::Instance()
{
	static ISSettingsDatabase SettingsDatabase;
	return SettingsDatabase;
}
//-----------------------------------------------------------------------------
QString ISSettingsDatabase::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::Initialize(const QVariantMap &VariantMap)
{
	for (const auto &MapItem : VariantMap.toStdMap())
	{
		Settings[MapItem.first] = MapItem.second;
	}
}
//-----------------------------------------------------------------------------
bool ISSettingsDatabase::Initialize()
{
	ISQueryModel QueryModel(ISMetaData::Instance().GetMetaTable("_SettingsDatabase"), ISNamespace::QMT_Object);
	QueryModel.SetClassFilter("sgdb_active");

	ISQuery qSelect(QueryModel.GetQueryText());
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		QSqlRecord SqlRecord = qSelect.GetRecord();
		for (int i = 0, c = SqlRecord.count(); i < c; ++i)
		{
			Settings[SqlRecord.field(i).name()] = SqlRecord.field(i).value();
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISSettingsDatabase::GetValue(const QString &SettingName)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	QVariant Value = Settings[SettingName];
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Value;
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::SetValue(const QString &SettingName, const QVariant &Value)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Settings[SettingName] = Value;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
