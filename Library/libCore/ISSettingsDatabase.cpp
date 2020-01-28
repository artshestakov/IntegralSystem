#include "ISSettingsDatabase.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISDebug.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SETTING = PREPARE_QUERY("SELECT column_name "
												 "FROM information_schema.columns "
												 "WHERE table_catalog = current_database() "
												 "AND table_schema = current_schema() "
												 "AND table_name = '_settingsdatabase'");
//-----------------------------------------------------------------------------
static QString QS_SYSTEM_PARAMETERS = PREPARE_QUERY("SELECT spdb_uid, spdb_value "
													"FROM _systemparameters "
													"WHERE NOT spdb_isdeleted "
													"ORDER BY spdb_id");
//-----------------------------------------------------------------------------
ISSettingsDatabase::ISSettingsDatabase()
{
	
}
//-----------------------------------------------------------------------------
ISSettingsDatabase::~ISSettingsDatabase()
{

}
//-----------------------------------------------------------------------------
ISSettingsDatabase& ISSettingsDatabase::GetInstance()
{
	static ISSettingsDatabase SettingsDatabase;
	return SettingsDatabase;
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::Initialize()
{
	ISCountingTime CountingTime;
	QVectorString VectorString;
	ISQuery qSelectColumn(QS_COLUMN_SETTING);
	qSelectColumn.SetShowLongQuery(false);
	if (qSelectColumn.Execute())
	{
		while (qSelectColumn.Next())
		{
			VectorString.append(qSelectColumn.ReadColumn("column_name").toString());
		}
	}

	QString SqlText = "SELECT \n";
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable("_SettingsDatabase");
	for (int i = 0; i < MetaTable->GetAllFields().count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->GetAllFields().at(i);
		QString FieldName = MetaTable->GetAlias() + "_" + MetaField->GetName().toLower();

		if (VectorString.contains(FieldName))
		{
			SqlText += FieldName + " AS \"" + MetaField->GetName() + "\", \n";
			VectorString[VectorString.indexOf(FieldName)] = MetaField->GetName();
		}
		else
		{
			ISDebug::ShowWarningString(QString("Not found column '%1' in table _SettingsDatabase").arg(FieldName));
			VectorString.removeOne(FieldName);
		}
	}

	ISSystem::RemoveLastSymbolFromString(SqlText, 3);
	SqlText += " \nFROM _settingsdatabase \n";
	SqlText += "WHERE sgdb_uid = :UID";

	//Запрос значений
	ISQuery qSelectValues(SqlText);
	qSelectValues.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectValues.ExecuteFirst())
	{
		for (const QString &String : VectorString)
		{
			try
			{
				QVariant SettingValue = qSelectValues.ReadColumn(String);
				if (SettingValue.isNull())
				{
					SettingValue.clear();
				}
				Settings.insert(String, SettingValue);
			}
			catch (ISQueryException &QueryException) { }
		}
	}

	ISDebug::ShowDebugString(QString("Initilize SettingsDatabase %1 msec").arg(CountingTime.GetElapsed()));
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::InitializedSystemParameters()
{
	ISQuery qSelect(QS_SYSTEM_PARAMETERS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			SystemParameters.insert(qSelect.ReadColumn("spdb_uid"), qSelect.ReadColumn("spdb_value"));
		}
	}
}
//-----------------------------------------------------------------------------
bool ISSettingsDatabase::GetValueBool(const QString &SettingName)
{
	return GetValue(SettingName).toBool();
}
//-----------------------------------------------------------------------------
QString ISSettingsDatabase::GetValueString(const QString &SettingName)
{
	return GetValue(SettingName).toString();
}
//-----------------------------------------------------------------------------
int ISSettingsDatabase::GetValueInt(const QString &SettingName)
{
	return GetValue(SettingName).toInt();
}
//-----------------------------------------------------------------------------
QVariant ISSettingsDatabase::GetValueDB(const QString &SettingName)
{
	QVariant Value;
	ISQuery qSelectValue(QString("SELECT sgdb_%1 FROM _settingsdatabase WHERE sgdb_uid = :UID").arg(SettingName));
	qSelectValue.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	qSelectValue.SetShowLongQuery(false);
	if (qSelectValue.ExecuteFirst())
	{
		Value = qSelectValue.ReadColumn("sgdb_" + SettingName);
	}

	return Value;
}
//-----------------------------------------------------------------------------
QVariant ISSettingsDatabase::GetValue(const QString &SettingName)
{
	return Settings.value(SettingName);
}
//-----------------------------------------------------------------------------
QVariant ISSettingsDatabase::GetSystemParameter(const ISUuid &UID)
{
	IS_ASSERT(SystemParameters.contains(UID), QString("Not found system parameter with uid: %1").arg(UID));
	return SystemParameters.value(UID);
}
//-----------------------------------------------------------------------------
