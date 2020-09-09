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
	
}
//-----------------------------------------------------------------------------
ISSettingsDatabase::~ISSettingsDatabase()
{

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
	return Settings[SettingName];
}
//-----------------------------------------------------------------------------
