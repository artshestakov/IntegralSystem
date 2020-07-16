#include "ISSettingsDatabase.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
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
	ISVectorString VectorString;
	ISQuery qSelectColumn(QS_COLUMN_SETTING);
	qSelectColumn.SetShowLongQuery(false);
	if (qSelectColumn.Execute())
	{
		while (qSelectColumn.Next())
		{
			VectorString.emplace_back(qSelectColumn.ReadColumn("column_name").toString());
		}
	}

	QString SqlText = "SELECT \n";
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable("_SettingsDatabase");
	for (PMetaField *MetaField : MetaTable->AllFields)
	{
		QString FieldName = MetaTable->Alias + '_' + MetaField->Name.toLower();
		if (ISAlgorithm::VectorContains(VectorString, FieldName))
		{
			SqlText += FieldName + " AS \"" + MetaField->Name + "\", \n";
			VectorString[ISAlgorithm::VectorIndexOf(VectorString, FieldName)] = MetaField->Name;
		}
		else
		{
			ISLOGGER_W(QString("Not found column '%1' in table _SettingsDatabase").arg(FieldName));
			ISAlgorithm::VectorRemoveAll(VectorString, FieldName);
		}
	}
	SqlText.chop(3);
	SqlText += " \nFROM _settingsdatabase \n";
	SqlText += "WHERE sgdb_uid = :UID";

	//������ ��������
	ISQuery qSelectValues(SqlText);
	qSelectValues.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectValues.ExecuteFirst())
	{
		for (const QString &String : VectorString)
		{
			QVariant SettingValue = qSelectValues.ReadColumn(String);
			if (SettingValue.isNull())
			{
				QVariant DefaultValue = MetaTable->GetField(String)->DefaultValueWidget;
				if (DefaultValue.isNull())
				{
					SettingValue.clear();
				}
				else
				{
					SettingValue = DefaultValue;
				}
			}
			Settings.emplace(String, SettingValue);
		}
	}
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::InitializedSystemParameters()
{
	ISQuery qSelect(QS_SYSTEM_PARAMETERS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			SystemParameters.emplace(qSelect.ReadColumn("spdb_uid"), qSelect.ReadColumn("spdb_value"));
		}
	}
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
	return Settings[SettingName];
}
//-----------------------------------------------------------------------------
QVariant ISSettingsDatabase::GetSystemParameter(const ISUuid &UID)
{
	IS_ASSERT(SystemParameters.count(UID), QString("Not found system parameter with uid: %1").arg(UID));
	return SystemParameters[UID];
}
//-----------------------------------------------------------------------------
