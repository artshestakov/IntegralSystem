#include "CGConfiguratorCreate.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "CGResource.h"
#include "ISMetaData.h"
#include "ISConfig.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QC_DATABASE = "CREATE DATABASE %1 WITH OWNER = %2 ENCODING = 'UTF8'";
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
CGConfiguratorCreate::CGConfiguratorCreate() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorCreate::~CGConfiguratorCreate()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorCreate::database()
{
	QString ErrorString;
	if (ISDatabase::GetInstance().ConnectToSystemDB(ErrorString))
	{
		QString DatabaseName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
		bool Exist = ISDatabase::GetInstance().CheckExistDatabase(DatabaseName);
		if (Exist)
		{
			ISDebug::ShowString("Database \"" + DatabaseName + "\" already exist");
		}
		else //Если база данных не существует - создать её
		{
			ISDebug::ShowString("Configurator.Creating database: " + DatabaseName);
			QSqlQuery SqlQuery = ISDatabase::GetInstance().GetSystemDB().exec(QC_DATABASE.arg(DatabaseName).arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN))); //Исполнение запроса на создание базы данных
			QSqlError SqlError = SqlQuery.lastError();
			Exist = SqlError.type() == QSqlError::NoError;
			Exist ? ISDebug::ShowString("The \"" + DatabaseName + "\" database was created successfully. It is recommended that you run the \"update database\" command")
				: ISDebug::ShowString("Error creating database \"" + DatabaseName + "\": " + SqlError.text());
		}

		if (Exist) //Если база данных создана
		{
			QString ErrorString;
			if (!ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString))
			{
				Exist = false;
			}
		}
		ISDatabase::GetInstance().DisconnectFromSystemDB();
	}
	else
	{
		ISDebug::ShowString(ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorCreate::systemuser()
{
	ISQuery qSelect(QS_SYSTEM_USER);
	qSelect.BindValue(":UID", CONST_UID_USER_POSTGRES);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			ISQuery qUpdate(QU_SYSTEM_USER);
			qUpdate.BindValue(":IsSystem", true);
			qUpdate.BindValue(":Surname", SYSTEM_USER_SURNAME);
			qUpdate.BindValue(":Name", SYSTEM_USER_NAME);
			qUpdate.BindValue(":Patronymic", SYSTEM_USER_PATRONYMIC);
			qUpdate.BindValue(":SexUID", CONST_UID_SEX_MALE);
			qUpdate.BindValue(":Login", CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
			qUpdate.BindValue(":AccessAllowed", true);
			qUpdate.BindValue(":UID", CONST_UID_USER_POSTGRES);
			if (qUpdate.Execute())
			{
				ISDebug::ShowString("System user updated");
			}
		}
		else
		{
			ISQuery qInsert(QI_SYSTEM_USER);
			qInsert.BindValue(":IsSystem", true);
			qInsert.BindValue(":UID", CONST_UID_USER_POSTGRES);
			qInsert.BindValue(":Surname", SYSTEM_USER_SURNAME);
			qInsert.BindValue(":Name", SYSTEM_USER_NAME);
			qInsert.BindValue(":Patronymic", SYSTEM_USER_PATRONYMIC);
			qInsert.BindValue(":SexUID", CONST_UID_SEX_MALE);
			qInsert.BindValue(":Login", CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
			qInsert.BindValue(":AccessAllowed", true);
			if (qInsert.Execute())
			{
				ISDebug::ShowString("System user created");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorCreate::databasesettings()
{
	ISQuery qSelect(QS_SETTINGS_DATABASE);
	qSelect.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			ISQuery qUpdate(QU_SETTINGS_DATABASE);
			qUpdate.BindValue(":IsSystem", true);
			qUpdate.BindValue(":SettingName", SETTING_DATABASE_NAME);
			qUpdate.BindValue(":SystemUserUID", CONST_UID_USER_POSTGRES);
			qUpdate.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
			if (qUpdate.Execute())
			{
				ISDebug::ShowString("Database settings updated");
			}
		}
		else
		{
			ISQuery qInsert(QI_SETTINGS_DATABASE);
			qInsert.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
			qInsert.BindValue(":IsSystem", true);
			qInsert.BindValue(":SettingName", SETTING_DATABASE_NAME);
			qInsert.BindValue(":SystemUserUID", CONST_UID_USER_POSTGRES);
			if (qInsert.Execute())
			{
				ISDebug::ShowString("Database settings created");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorCreate::resources()
{
	int CountResources = ISMetaData::GetInstanse().GetResources().count();
	for (int i = 0; i < CountResources; ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);
		Progress("Resources for " + MetaResource->TableName, i, CountResources);

		QString ErrorString;
		if (!CGResource::CheckExistResource(MetaResource))
		{
			CGResource::InsertResource(MetaResource, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
