#include "CGConfiguratorUpdate.h"
#include "ISMetaData.h"
#include "CGDatabase.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISConsole.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_SYSTEM_USER = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _users "
											  "WHERE usrs_uid = :UID");
//-----------------------------------------------------------------------------
static QString QU_SYSTEM_USER = PREPARE_QUERY("UPDATE _users SET "
											  "usrs_issystem = true, "
											  "usrs_fio = :FIO, "
											  "usrs_login = :Login, "
											  "usrs_accessallowed = true, "
											  "usrs_photo = :Photo "
											  "WHERE usrs_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_SYSTEM_USER = PREPARE_QUERY("INSERT INTO _users(usrs_uid, usrs_issystem, usrs_fio, usrs_login, usrs_hash, usrs_salt, usrs_accessallowed, usrs_photo) "
											  "VALUES(:UID, true, :FIO, :Login, :Hash, :Salt, true, :Photo)");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT COUNT(*) "
													"FROM _settingsdatabase "
													"WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static QString QU_SETTINGS_DATABASE = PREPARE_QUERY("UPDATE _settingsdatabase SET "
													"sgdb_issystem = true, "
													"sgdb_active = true, "
													"sgdb_settingname = :SettingName "
													"WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static QString QI_SETTINGS_DATABASE = PREPARE_QUERY("INSERT INTO _settingsdatabase(sgdb_uid, sgdb_issystem, sgdb_active, sgdb_settingname) "
													"VALUES(:UID, true, true, :SettingName)");
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
		Result = comment();
	}

	if (Result)
	{
		Result = indexes();
	}

	if (Result)
	{
		Result = foreigns();
	}
	
	if (Result)
	{
		Result = resources();
	}
	
	/*if (Result)
	{
		Result = systemuser();
	}*/

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

	//������� ������� �� ����-������
	for (size_t i = 0, CountFunctions = ISMetaData::Instance().GetFunctions().size(); i < CountFunctions; ++i)
	{
		Progress("Function", i, CountFunctions);
		PMetaFunction *MetaFunction = ISMetaData::Instance().GetFunctions()[i];
		Result = CGDatabase::Function_Create(MetaFunction, ErrorString);
		if (!Result)
		{
			break;
		}
	}

	if (Result) //���� ���������� ������� ������ ������� - ������� ����������
	{
		Result = CGDatabase::Function_Delete(ErrorString);
	}
	
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::tables()
{
	bool Result = true, Exist = true;
	for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i) //����� ������
	{
		PMetaTable *MetaTable = ISMetaData::Instance().GetTables()[i];
		Progress("Table", i, CountTables, "TableName: " + MetaTable->Name);
		Result = CGDatabase::Table_Exist(MetaTable, Exist, ErrorString);
		if (Result)
		{
			Result = Exist ? CGDatabase::Table_Update(MetaTable, ErrorString) : CGDatabase::Table_Create(MetaTable, ErrorString);
		}
		
		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::comment()
{
	bool Result = true;
	for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i) //����� ������
	{
		PMetaTable *MetaTable = ISMetaData::Instance().GetTables()[i];
		Progress("Comment Table", i, CountTables, "TableName: " + MetaTable->Name);
		Result = CGDatabase::Helper_CommentTable(MetaTable, ErrorString);

		if (Result) //��������������� ������� ������ ������� - ������������ ����
		{
			for (size_t j = 0, CountFields = MetaTable->Fields.size(); j < CountFields; ++j)
			{
				PMetaField *MetaField = MetaTable->Fields[j];
				Progress("Comment Field", j, CountFields, "FieldName: " + MetaField->Name);
				Result = CGDatabase::Helper_CommentField(MetaTable, MetaField, ErrorString);
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
bool CGConfiguratorUpdate::indexes()
{
	bool Result = true, Exist = true;
	std::vector<PMetaIndex*> Indexes = ISMetaData::Instance().GetIndexes();
	for (size_t i = 0, CountIndexes = Indexes.size(); i < CountIndexes; ++i) //����� ��������
	{
		PMetaIndex *MetaIndex = Indexes[i];
		Progress("Index", i, CountIndexes, "Table: " + MetaIndex->TableName + ". IndexName: " + MetaIndex->GetName());
		Result = CGDatabase::Index_Exist(MetaIndex, Exist, ErrorString);
		if (Result) //���� �������� ������������� ������� ������ ������� � ��� � �� ��� - ������
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
bool CGConfiguratorUpdate::foreigns()
{
	bool Result = true, Exist = true;
	for (size_t i = 0, CountForeigns = ISMetaData::Instance().GetForeigns().size(); i < CountForeigns; ++i)
	{
		PMetaForeign *MetaForeign = ISMetaData::Instance().GetForeigns()[i];
		Progress("Foreign", i, CountForeigns, "Table: " + MetaForeign->TableName + ". ForeignName: " + MetaForeign->GetName());
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

	if (Result) //���� ���������� �������� ������ ������� - ������� ����������
	{
		ISVectorString Tables; //�������, ������� ������� ������������ � ����-������
		for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
		{
			if (!ISAlgorithm::VectorContains(Tables, MetaResource->TableName))
			{
				Tables.emplace_back(MetaResource->TableName);
			}
		}
		
		for (const QString &TableName : Tables) //������� �������
		{
			QString TableAlias = ISMetaData::Instance().GetMetaTable(TableName)->Alias;

			ISQuery qSelectResources(QString("SELECT %1_uid AS uid FROM %2 WHERE %1_issystem").arg(TableAlias).arg(TableName));
			Result = qSelectResources.Execute();
			if (Result)
			{
				while (qSelectResources.Next()) //������� ������� ���������� �������
				{
					ISUuid ResourceUID = qSelectResources.ReadColumn("uid");
					if (!ISMetaData::Instance().CheckExistResource(ResourceUID)) //���� ������ �� ���������� � ����-������ - ���������� ������� ��� �� ��
					{
						if (ISConsole::Question(QString("Delete old resource %1 in table %2?").arg(ResourceUID).arg(TableName))) //������������ ���������� ������� ������ �� ��
						{
							ISQuery qDeleteResource;
							if (!qDeleteResource.Execute(QString("DELETE FROM %1 WHERE %2_uid = '%3'").arg(TableName).arg(TableAlias).arg(ResourceUID)))
							{
								ISDEBUG_L(QString("Error delete resource %1: %2").arg(ResourceUID).arg(qDeleteResource.GetErrorString()));
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
	qSelect.BindValue(":UID", SYSTEM_USER_UID);
	qSelect.SetShowLongQuery(false);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		//������ ���� � ���������
		QByteArray ByteArray;
		QFile FileAvatar(":/Other/AdminAvatar.png");
		if (FileAvatar.open(QIODevice::ReadOnly))
		{
			ByteArray = FileAvatar.readAll();
			FileAvatar.close();
		}
		else
		{
			ISDEBUG_W("Error open avatar \"" + FileAvatar.fileName() + "\": " + FileAvatar.errorString());
		}

		if (qSelect.ReadColumn("count").toInt() == 0) //������������ ��� �� ���������� - ������
		{
			//������ ������ ������
			QString Password;
			while (true)
			{
				Password = ISConsole::GetString("Create password for system administrator: ");
				if (ISAlgorithm::PasswordVerification(Password))
				{
					ISDEBUG_L("WARNING! Keep password in a safe place.");
					break;
				}
				else
				{
					ISDEBUG_L("Invalid password!");
				}
			}

			//��������� ���, ���������� ���� � ����� ������
			QString Hash = ISSystem::StringToSha256(SYSTEM_USER_LOGIN + Password), Salt;
			if (!ISAlgorithm::GenerateSalt(Salt, ErrorString))
			{
				return false;
			}
			QString HashResult = ISAlgorithm::SaltPassword(Hash, Salt);

			//��������� � ��
			ISQuery qInsert(QI_SYSTEM_USER);
			qInsert.BindValue(":UID", SYSTEM_USER_UID);
			qInsert.BindValue(":FIO", QString::fromLocal8Bit("������� ������������� �������"));
			qInsert.BindValue(":Login", SYSTEM_USER_LOGIN);
			qInsert.BindValue(":Hash", HashResult);
			qInsert.BindValue(":Salt", Salt);
			qInsert.BindValue(":Photo", ByteArray);
			qInsert.SetShowLongQuery(false);
			Result = qInsert.Execute();
			if (Result)
			{
				ISDEBUG_L("System user created!");
			}
			else
			{
				ErrorString = qInsert.GetErrorString();
			}
		}
		else //������������ ��� ���������� - ���������
		{
			ISQuery qUpdate(QU_SYSTEM_USER);
			qUpdate.BindValue(":FIO", QString::fromLocal8Bit("������� ������������� �������"));
			qUpdate.BindValue(":Login", SYSTEM_USER_LOGIN);
			qUpdate.BindValue(":Photo", ByteArray);
			qUpdate.BindValue(":UID", SYSTEM_USER_UID);
			qUpdate.SetShowLongQuery(false);
			Result = qUpdate.Execute();
			if (Result)
			{
				ISDEBUG_L("System user updated!");
			}
			else
			{
				ErrorString = qUpdate.GetErrorString();
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
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		bool IsExist = qSelect.ReadColumn("count").toInt() > 0;
		ISQuery qUpsert(IsExist ? QU_SETTINGS_DATABASE : QI_SETTINGS_DATABASE);
		qUpsert.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
		qUpsert.BindValue(":SettingName", "System Profile");
		qUpsert.SetShowLongQuery(false);
		Result = qUpsert.Execute();
		if (Result)
		{
			ISDEBUG_L(IsExist ? "Database settings updated" : "Database settings created");
		}
		else
		{
			ErrorString = qUpsert.GetErrorString();
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
