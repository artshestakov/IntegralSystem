#include "CGConfiguratorUpdate.h"
#include "ISMetaData.h"
#include "CGDatabase.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConsole.h"
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
													"sgdb_administrator = (SELECT usrs_oid FROM _users WHERE usrs_uid = :SystemUserUID) "
													"WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_SETTINGS_DATABASE = PREPARE_QUERY("INSERT INTO _settingsdatabase(sgdb_uid, sgdb_issystem, sgdb_settingname, sgdb_useraccessdatabase, sgdb_administrator) "
													"VALUES(:UID, :IsSystem, :SettingName, :UserAccessDatabase, (SELECT usrs_oid FROM _users WHERE usrs_uid = :SystemUserUID))");
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
	for (size_t i = 0, CountFunctions = ISMetaData::Instance().GetFunctions().size(); i < CountFunctions; ++i)
	{
		Progress("Function", i, CountFunctions);
		PMetaFunction *MetaFunction = ISMetaData::Instance().GetFunctions()[i];
		Result = CGDatabase::Function_CreateOrReplace(MetaFunction, ErrorString);
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
	for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i) //Обход таблиц
	{
		PMetaTable *MetaTable = ISMetaData::Instance().GetTables()[i];
		Progress("Table", i, CountTables, "TableName: " + MetaTable->Name);
		Result = CGDatabase::Table_Exist(MetaTable, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGDatabase::Table_Update(MetaTable, ErrorString) : CGDatabase::Table_Create(MetaTable, ErrorString);
		}

		if (Result) //Создание/обновление таблицы прошло успешно - комментируем таблицу
		{
			Result = CGDatabase::Helper_CommentTable(MetaTable, ErrorString);
		}

		if (Result) //Комментируем поля таблицы
		{
			for (PMetaField *MetaField : MetaTable->AllFields)
			{
				if (MetaField->QueryText.isEmpty()) //Если поле является обычным полем - комментируем его
				{
					Result = CGDatabase::Helper_CommentField(MetaTable, MetaField, ErrorString);
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
	for (size_t i = 0, CountIndexes = ISMetaData::Instance().GetSystemIndexes().size(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaIndex *MetaIndex = ISMetaData::Instance().GetSystemIndexes()[i];
		Progress("System index", i, CountIndexes, "Table: " + MetaIndex->TableName + ". IndexName: " + MetaIndex->GetName());
		if (CGDatabase::Index_Exist(MetaIndex, Exist, ErrorString))
		{
			if (Exist)
			{
				if (MetaIndex->FieldName.toLower() == "id") //Если поле primary_key - делать reindex
				{
					Result = CGDatabase::Index_ReIndex(MetaIndex, ErrorString);
				}
				else if (CGDatabase::Index_CheckForeign(MetaIndex)) //Если на поле, где установлен текущий индекс ссылается внешний ключ - делать reindex
				{
					Result = CGDatabase::Index_ReIndex(MetaIndex, ErrorString);
				}
				else
				{
					Result = CGDatabase::Index_Update(MetaIndex, ErrorString);
				}
			}
			else
			{
				Result = CGDatabase::Index_Create(MetaIndex, ErrorString);
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
	for (size_t i = 0, CountIndexes = ISMetaData::Instance().GetIndexes().size(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaIndex *MetaIndex = ISMetaData::Instance().GetIndexes()[i];
		Progress("Index", i, CountIndexes, "Table: " + MetaIndex->TableName + ". IndexName: " + MetaIndex->GetName());
		Result = CGDatabase::Index_Exist(MetaIndex, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGDatabase::Index_Update(MetaIndex, ErrorString) : CGDatabase::Index_Create(MetaIndex, ErrorString);
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
	for (size_t i = 0, CountIndexes = ISMetaData::Instance().GetCompoundIndexes().size(); i < CountIndexes; ++i)
	{
		PMetaIndex *MetaIndex = ISMetaData::Instance().GetCompoundIndexes()[i];
		Progress("Compound index", i, CountIndexes, "Table: " + MetaIndex->TableName + ". IndexName: " + MetaIndex->GetName());
		Result = CGDatabase::Index_Exist(MetaIndex, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGDatabase::Index_Update(MetaIndex, ErrorString) : CGDatabase::Index_Create(MetaIndex, ErrorString);
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
	for (size_t i = 0, CountForeigns = ISMetaData::Instance().GetForeigns().size(); i < CountForeigns; ++i)
	{
		PMetaForeign *MetaForeign = ISMetaData::Instance().GetForeigns()[i];
		Progress("Foreign", i, CountForeigns, "Table: " + MetaForeign->TableName + ". ForeignName:" + CGDatabase::Foreign_GetName(MetaForeign));
		Result = CGDatabase::Foreign_Exist(MetaForeign, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGDatabase::Foreign_Update(MetaForeign, ErrorString) : CGDatabase::Foreign_Create(MetaForeign, ErrorString);
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
	for (size_t i = 0, CountResources = ISMetaData::Instance().GetResources().size(); i < CountResources; ++i)
	{
		PMetaResource *MetaResource = ISMetaData::Instance().GetResources()[i];
		Progress("Resource", i, CountResources, "UID: " + MetaResource->UID);
		
		if (CGDatabase::Resource_Exist(MetaResource, Exist, ErrorString))
		{
			Result = Exist ? CGDatabase::Resource_Update(MetaResource, ErrorString) : CGDatabase::Resource_Insert(MetaResource, ErrorString);
		}

		if (!Result)
		{
			break;
		}
	}

	if (Result) //Если обновление ресурсов прошло успешно - удаляем устаревшие
	{
		std::vector<QString> Tables; //Таблицы, ресурсы которых присутствуют в мета-данных
		for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
		{
			if (!ISAlgorithm::VectorContains(Tables, MetaResource->TableName))
			{
				Tables.emplace_back(MetaResource->TableName);
			}
		}
		
		for (const QString &TableName : Tables) //Обходим таблицы
		{
			QString TableAlias = ISMetaData::Instance().GetMetaTable(TableName)->Alias;

			ISQuery qSelectResources(QString("SELECT %1_uid AS uid FROM %2 WHERE %1_issystem").arg(TableAlias).arg(TableName));
			Result = qSelectResources.Execute();
			if (Result)
			{
				while (qSelectResources.Next()) //Обходим ресурсы конкретной таблицы
				{
					ISUuid ResourceUID = qSelectResources.ReadColumn("uid");
					if (!ISMetaData::Instance().CheckExistResource(ResourceUID)) //Если ресурс не существует в мета-данных - предлагаем удалить его из БД
					{
						if (ISConsole::Question(QString("Delete old resource %1 in table %2?").arg(ResourceUID).arg(TableName))) //Пользователь согласился удалить ресурс из БД
						{
							ISQuery qDeleteResource;
							if (!qDeleteResource.Execute(QString("DELETE FROM %1 WHERE %2_uid = '%3'").arg(TableName).arg(TableAlias).arg(ResourceUID)))
							{
								ISLOGGER_L(QString("Error delete resource %1: %2").arg(ResourceUID).arg(qDeleteResource.GetErrorString()));
							}
						}
					}
				}
			}
			else
			{
				ErrorString = "Error select resources: " + qSelectResources.GetErrorString();
				break;
			}
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
