#include "CGConfiguratorUpdate.h"
#include "ISMetaData.h"
#include "CGFunction.h"
#include "CGTable.h"
#include "CGIndex.h"
#include "CGForeign.h"
#include "CGResource.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_SYSTEM_USER = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _users "
											  "WHERE usrs_uid = :UID");
//-----------------------------------------------------------------------------
static QString QU_SYSTEM_USER = PREPARE_QUERY("UPDATE _users SET "
											  "usrs_issystem = :IsSystem, "
											  "usrs_surname = :Surname, "
											  "usrs_name = :Name, "
											  "usrs_patronymic = :Patronymic, "
											  "usrs_sex = (SELECT sexs_id FROM _sex WHERE sexs_uid = :SexUID),"
											  "usrs_login = :Login, "
											  "usrs_accessallowed = :AccessAllowed "
											  "WHERE usrs_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_SYSTEM_USER = PREPARE_QUERY("INSERT INTO _users(usrs_issystem, usrs_uid, usrs_surname, usrs_name, usrs_patronymic, usrs_sex, usrs_login, usrs_accessallowed) "
											  "VALUES(:IsSystem, :UID, :Surname, :Name, :Patronymic, (SELECT sexs_id FROM _sex WHERE sexs_uid = :SexUID), :Login, :AccessAllowed)");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT COUNT(*) "
													"FROM _settingsdatabase "
													"WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static QString QU_SETTINGS_DATABASE = PREPARE_QUERY("UPDATE _settingsdatabase SET "
													"sgdb_issystem = :IsSystem, "
													"sgdb_settingname = :SettingName, "
													"sgdb_useraccessdatabase = :UserAccessDatabase, "
													"sgdb_administrator = (SELECT usrs_id FROM _users WHERE usrs_uid = :SystemUserUID) "
													"WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_SETTINGS_DATABASE = PREPARE_QUERY("INSERT INTO _settingsdatabase(sgdb_uid, sgdb_issystem, sgdb_settingname, sgdb_useraccessdatabase, sgdb_administrator) "
													"VALUES(:UID, :IsSystem, :SettingName, :UserAccessDatabase, (SELECT usrs_id FROM _users WHERE usrs_uid = :SystemUserUID))");
//-----------------------------------------------------------------------------
CGConfiguratorUpdate::CGConfiguratorUpdate() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorUpdate::~CGConfiguratorUpdate()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::database()
{
	functions();
	tables();
	systemindexes();
	indexes();
	compoundindexes();
	foreigns();
	resources();
	systemuser();
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::functions()
{
	ISLOGGER_DEBUG("Updating functions...");
	for (int i = 0, CountFunctions = ISMetaData::GetInstanse().GetFunctions().size(); i < CountFunctions; ++i)
	{
		Progress("Functions", i, CountFunctions);
		QString ErrorString;
		PMetaClassFunction *MetaFunction = ISMetaData::GetInstanse().GetFunctions()[i];
		/*if (!CGFunction::CheckDependencies(MetaFunction))
		{
			if (CGFunction::CheckExist(MetaFunction, ErrorString))
			{
				CGFunction::Delete(MetaFunction);
			}
		}*/
		CGFunction::CreateOrReplaceFunction(MetaFunction, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::tables()
{
	ISLOGGER_DEBUG("Updating tables...");
	for (int i = 0, CountTables = ISMetaData::GetInstanse().GetTables().size(); i < CountTables; ++i) //Обход таблиц
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		Progress(QString("Table: %1").arg(MetaTable->Name), i, CountTables);

		QString ErrorString;
		CGTable::CheckExistTable(MetaTable) ? CGTable::UpdateTable(MetaTable) : CGTable::CreateTable(MetaTable, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::systemindexes()
{
	ISLOGGER_DEBUG("Updating system indexes...");
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetSystemIndexes().size(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetSystemIndexes().at(i);
		Progress(QString("System indexes for: %1").arg(MetaIndex->TableName), i, CountIndexes);
		
		QString ErrorString;
		if (CGIndex::CheckExistIndex(MetaIndex))
		{
			if (MetaIndex->FieldName.toLower() == "id") //Если поле primary_key - делать reindex
			{
                CGIndex::ReindexIndex(MetaIndex, ErrorString);
			}
			else if (CGIndex::CheckIndexForeign(MetaIndex)) //Если на поле, где установлен текущий индекс ссылается внешний ключ - делать reindex
			{
                CGIndex::ReindexIndex(MetaIndex, ErrorString);
			}
			else
			{
                CGIndex::UpdateIndex(MetaIndex, ErrorString);
			}
		}
		else
		{
            CGIndex::CreateIndex(MetaIndex, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::indexes()
{
	ISLOGGER_DEBUG("Updating indexes...");
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetIndexes().size(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetIndexes().at(i);
		Progress(QString("Indexes for: %1").arg(MetaIndex->TableName), i, CountIndexes);

		QString ErrorString;
		CGIndex::CheckExistIndex(MetaIndex) ? CGIndex::UpdateIndex(MetaIndex, ErrorString) : CGIndex::CreateIndex(MetaIndex, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::compoundindexes()
{
	ISLOGGER_DEBUG("Updating compound indexes...");
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetCompoundIndexes().size(); i < CountIndexes; ++i)
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetCompoundIndexes().at(i);
		Progress(QString("Compound indexes for: %1").arg(MetaIndex->TableName), i, CountIndexes);

		QString ErrorString;
		CGIndex::CheckExistIndex(MetaIndex) ? CGIndex::UpdateIndex(MetaIndex, ErrorString) : CGIndex::CreateIndex(MetaIndex, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::foreigns()
{
	ISLOGGER_DEBUG("Updating foreigns...");
	for (int i = 0, CountForeigns = ISMetaData::GetInstanse().GetForeigns().size(); i < CountForeigns; ++i)
	{
		Progress("Foreigns", i, CountForeigns);
		PMetaClassForeign *MetaForeign = ISMetaData::GetInstanse().GetForeigns().at(i);
		QString ErrorString;
		CGForeign::CheckExistForeign(MetaForeign) ? CGForeign::UpdateForeign(MetaForeign, ErrorString) : CGForeign::CreateForeign(MetaForeign, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::resources()
{
	ISLOGGER_DEBUG("Updating resources...");
	for (int i = 0, CountResources = ISMetaData::GetInstanse().GetResources().size(); i < CountResources; ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);
		Progress(QString("Resources for: %1").arg(MetaResource->TableName), i, CountResources);
		
		QString ErrorString;
		CGResource::CheckExistResource(MetaResource) ? CGResource::UpdateResource(MetaResource) : CGResource::InsertResource(MetaResource, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::systemuser()
{
	ISQuery qSelect(QS_SYSTEM_USER);
	qSelect.BindValue(":UID", CONST_UID_USER_POSTGRES);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			ISQuery qUpdate(QU_SYSTEM_USER);
			qUpdate.BindValue(":IsSystem", true);
			qUpdate.BindValue(":Surname", LANG("SystemUser.Surname"));
			qUpdate.BindValue(":Name", LANG("SystemUser.Name"));
			qUpdate.BindValue(":Patronymic", LANG("SystemUser.Patronymic"));
			qUpdate.BindValue(":SexUID", CONST_UID_SEX_MALE);
			qUpdate.BindValue(":Login", CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
			qUpdate.BindValue(":AccessAllowed", true);
			qUpdate.BindValue(":UID", CONST_UID_USER_POSTGRES);
			if (qUpdate.Execute())
			{
				ISLOGGER_UNKNOWN("System user updated");
			}
		}
		else
		{
			ISQuery qInsert(QI_SYSTEM_USER);
			qInsert.BindValue(":IsSystem", true);
			qInsert.BindValue(":UID", CONST_UID_USER_POSTGRES);
			qInsert.BindValue(":Surname", LANG("SystemUser.Surname"));
			qInsert.BindValue(":Name", LANG("SystemUser.Name"));
			qInsert.BindValue(":Patronymic", LANG("SystemUser.Patronymic"));
			qInsert.BindValue(":SexUID", CONST_UID_SEX_MALE);
			qInsert.BindValue(":Login", CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
			qInsert.BindValue(":AccessAllowed", true);
			if (qInsert.Execute())
			{
				ISLOGGER_UNKNOWN("System user created");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::databasesettings()
{
	ISQuery qSelect(QS_SETTINGS_DATABASE);
	qSelect.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			ISQuery qUpdate(QU_SETTINGS_DATABASE);
			qUpdate.BindValue(":IsSystem", true);
			qUpdate.BindValue(":SettingName", LANG("DatabaseSettings.Name"));
			qUpdate.BindValue(":UserAccessDatabase", true);
			qUpdate.BindValue(":SystemUserUID", CONST_UID_USER_POSTGRES);
			qUpdate.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
			if (qUpdate.Execute())
			{
				ISLOGGER_UNKNOWN("Database settings updated");
			}
		}
		else
		{
			ISQuery qInsert(QI_SETTINGS_DATABASE);
			qInsert.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
			qInsert.BindValue(":IsSystem", true);
			qInsert.BindValue(":SettingName", LANG("DatabaseSettings.Name"));
			qInsert.BindValue(":UserAccessDatabase", true);
			qInsert.BindValue(":SystemUserUID", CONST_UID_USER_POSTGRES);
			if (qInsert.Execute())
			{
				ISLOGGER_UNKNOWN("Database settings created");
			}
		}
	}
}
//-----------------------------------------------------------------------------
