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
#include "CGHelper.h"
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
bool CGConfiguratorUpdate::database()
{
	bool Result = functions();
	if (Result)
	{
		Result = tables();
	}

	if (Result)
	{
		Result = indexesall();
	}

	if (Result)
	{
		Result = foreigns();
	}
	
	if (Result)
	{
		Result = resources();
	}
	
	if (Result)
	{
		Result = systemuser();
	}

	if (Result)
	{
		Result = databasesettings();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::functions()
{
	bool Result = true;
	for (int i = 0, CountFunctions = ISMetaData::GetInstanse().GetFunctions().size(); i < CountFunctions; ++i)
	{
		Progress("Function", i, CountFunctions);
		PMetaFunction *MetaFunction = ISMetaData::GetInstanse().GetFunctions()[i];
		Result = CGFunction::CreateOrReplaceFunction(MetaFunction, ErrorString);
		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::tables()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountTables = ISMetaData::GetInstanse().GetTables().size(); i < CountTables; ++i) //Обход таблиц
	{
		PMetaTable *MetaTable = ISMetaData::GetInstanse().GetTables()[i];
		Progress("Table", i, CountTables, "TableName: " + MetaTable->Name);
		Result = CGTable::CheckExistTable(MetaTable, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGTable::UpdateTable(MetaTable, ErrorString) : CGTable::CreateTable(MetaTable, ErrorString);
		}

		if (Result) //Создание/обновление таблицы прошло успешно - комментируем таблицу
		{
			Result = CGHelper::CommentTable(MetaTable->Name.toLower(), MetaTable->LocalListName, ErrorString);
		}

		if (Result) //Комментируем поля таблицы
		{
			for (PMetaField *MetaField : MetaTable->AllFields)
			{
				if (MetaField->QueryText.isEmpty()) //Если поле является обычным полем - комментируем его
				{
					Result = CGHelper::CommentField(MetaTable->Name.toLower(), MetaTable->Alias + '_' + MetaField->Name.toLower(), MetaField->LocalListName, ErrorString);
				}

				if (!Result)
				{
					break;
				}
			}
		}
		
		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::indexesall()
{
	bool Result = systemindexes();
	if (Result)
	{
		Result = indexes();
	}

	if (Result)
	{
		Result = compoundindexes();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::systemindexes()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetSystemIndexes().size(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaIndex *MetaIndex = ISMetaData::GetInstanse().GetSystemIndexes()[i];
		Progress("System index", i, CountIndexes, "Table: " + MetaIndex->TableName);
		if (CGIndex::CheckExistIndex(MetaIndex, Exist, ErrorString))
		{
			if (Exist)
			{
				if (MetaIndex->FieldName.toLower() == "id") //Если поле primary_key - делать reindex
				{
					Result = CGIndex::ReindexIndex(MetaIndex, ErrorString);
				}
				else if (CGIndex::CheckIndexForeign(MetaIndex)) //Если на поле, где установлен текущий индекс ссылается внешний ключ - делать reindex
				{
					Result = CGIndex::ReindexIndex(MetaIndex, ErrorString);
				}
				else
				{
					Result = CGIndex::UpdateIndex(MetaIndex, ErrorString);
				}
			}
			else
			{
				Result = CGIndex::CreateIndex(MetaIndex, ErrorString);
			}
		}

		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::indexes()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetIndexes().size(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaIndex *MetaIndex = ISMetaData::GetInstanse().GetIndexes()[i];
		Progress("Index", i, CountIndexes, "Table: " + MetaIndex->TableName);
		Result = CGIndex::CheckExistIndex(MetaIndex, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGIndex::UpdateIndex(MetaIndex, ErrorString) : CGIndex::CreateIndex(MetaIndex, ErrorString);
		}

		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::compoundindexes()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetCompoundIndexes().size(); i < CountIndexes; ++i)
	{
		PMetaIndex *MetaIndex = ISMetaData::GetInstanse().GetCompoundIndexes()[i];
		Progress("Compound index", i, CountIndexes, "Table: " + MetaIndex->TableName);
		Result = CGIndex::CheckExistIndex(MetaIndex, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGIndex::UpdateIndex(MetaIndex, ErrorString) : CGIndex::CreateIndex(MetaIndex, ErrorString);
		}
		
		if (!Result)
		{
			break;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::foreigns()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountForeigns = ISMetaData::GetInstanse().GetForeigns().size(); i < CountForeigns; ++i)
	{
		Progress("Foreign", i, CountForeigns);
		PMetaForeign *MetaForeign = ISMetaData::GetInstanse().GetForeigns()[i];
		Result = CGForeign::CheckExistForeign(MetaForeign, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGForeign::UpdateForeign(MetaForeign, ErrorString) : CGForeign::CreateForeign(MetaForeign, ErrorString);
		}

		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::resources()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountResources = ISMetaData::GetInstanse().GetResources().size(); i < CountResources; ++i)
	{
		PMetaResource *MetaResource = ISMetaData::GetInstanse().GetResources()[i];
		Progress("Resource", i, CountResources, "ResourceUID: " + MetaResource->UID);
		
		if (CGResource::CheckExistResource(MetaResource, Exist, ErrorString))
		{
			Result = Exist ? CGResource::UpdateResource(MetaResource, ErrorString) : CGResource::InsertResource(MetaResource, ErrorString);
		}

		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::systemuser()
{
	ISQuery qSelect(QS_SYSTEM_USER);
	qSelect.BindValue(":UID", CONST_UID_USER_POSTGRES);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		if (qSelect.ReadColumn("count").toInt() > 0)
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
			Result = qUpdate.Execute();
			if (Result)
			{
				ISLOGGER_L("System user updated");
			}
			else
			{
				ErrorString = qUpdate.GetErrorString();
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
			Result = qInsert.Execute();
			if (Result)
			{
				ISLOGGER_L("System user created");
			}
			else
			{
				ErrorString = qInsert.GetErrorString();
			}
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::databasesettings()
{
	ISQuery qSelect(QS_SETTINGS_DATABASE);
	qSelect.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		if (qSelect.ReadColumn("count").toInt() > 0)
		{
			ISQuery qUpdate(QU_SETTINGS_DATABASE);
			qUpdate.BindValue(":IsSystem", true);
			qUpdate.BindValue(":SettingName", LANG("DatabaseSettings.Name"));
			qUpdate.BindValue(":UserAccessDatabase", true);
			qUpdate.BindValue(":SystemUserUID", CONST_UID_USER_POSTGRES);
			qUpdate.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
			Result = qUpdate.Execute();
			if (Result)
			{
				ISLOGGER_L("Database settings updated");
			}
			else
			{
				ErrorString = qUpdate.GetErrorString();
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
			Result = qInsert.Execute();
			if (Result)
			{
				ISLOGGER_L("Database settings created");
			}
			else
			{
				ErrorString = qInsert.GetErrorString();
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
