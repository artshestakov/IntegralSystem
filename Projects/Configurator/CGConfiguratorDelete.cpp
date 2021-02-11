#include "CGConfiguratorDelete.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QU_ADMIN_PASSWORD = PREPARE_QUERY("UPDATE _users SET "
												 "usrs_hash = NULL, "
												 "usrs_salt = NULL "
												 "WHERE usrs_uid = :UID");
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT indexname "
										  "FROM pg_indexes "
										  "WHERE schemaname = current_schema() "
										  "AND right(indexname, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static QString QD_INDEX = "DROP INDEX public.%1 CASCADE";
//-----------------------------------------------------------------------------
static QString QS_FOREIGNS = PREPARE_QUERY("SELECT constraint_name "
										   "FROM information_schema.constraint_table_usage "
										   "WHERE table_catalog = current_database() "
										   "AND table_schema = current_schema() "
										   "AND right(constraint_name, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static QString QD_FOREIGN = "ALTER TABLE public.%1 DROP CONSTRAINT %2 RESTRICT";
//-----------------------------------------------------------------------------
static QString QS_TABLES = PREPARE_QUERY("SELECT tablename "
										 "FROM pg_tables "
										 "WHERE schemaname = current_schema() "
										 "ORDER BY tablename");
//-----------------------------------------------------------------------------
static QString QS_COLUMNS = PREPARE_QUERY("SELECT table_name, column_name "
										  "FROM information_schema.columns "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "ORDER BY table_name, ordinal_position");
//-----------------------------------------------------------------------------
static QString QS_SEQUENCES = PREPARE_QUERY("SELECT sequence_name "
											"FROM information_schema.sequences "
											"WHERE sequence_catalog = current_database() "
											"AND sequence_name NOT IN(:Where) "
											"ORDER BY sequence_name");
//-----------------------------------------------------------------------------
static QString QD_SEQUENCE = "DROP SEQUENCE public.%1";
//-----------------------------------------------------------------------------
static QString QS_INDEX = PREPARE_QUERY("SELECT indexname "
										"FROM pg_indexes "
										"JOIN pg_class c ON c.relname = indexname "
										"JOIN pg_index ON indexrelid = c.oid "
										"WHERE schemaname = current_schema() "
										"AND right(indexname, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static QString QS_FOREIGN = PREPARE_QUERY("SELECT table_name, constraint_name "
										  "FROM information_schema.table_constraints "
										  "WHERE constraint_type = 'FOREIGN KEY' "
										  "AND table_schema = current_schema() "
										  "ORDER BY constraint_name");
//-----------------------------------------------------------------------------
CGConfiguratorDelete::CGConfiguratorDelete() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorDelete::~CGConfiguratorDelete()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::adminpassword()
{
	if (ISConsole::Question("Are you sure?"))
	{
		ISQuery qUpdatePassword(QU_ADMIN_PASSWORD);
		qUpdatePassword.BindValue(":UID", SYSTEM_USER_UID);
		qUpdatePassword.SetShowLongQuery(false);
		bool Result = qUpdatePassword.Execute();
		if (Result) //Запрос прошёл успешно
		{
			Result = qUpdatePassword.GetCountAffected() > 0;
			if (Result) //Пароль был удалён
			{
				ISDEBUG_I("Password deleted successfully");
			}
			else //Админ в БД не существует - ошибка
			{
				ErrorString = "The administrator account does not exist";
			}
		}
		else //Ошибка запроса
		{
			ErrorString = qUpdatePassword.GetErrorString();
		}
		return Result;
	}
	ErrorString = "You refused";
	return false;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::indexes()
{
	ISQuery qSelectIndexes(QS_INDEXES);
	qSelectIndexes.SetShowLongQuery(false);
	bool Result = qSelectIndexes.Execute();
	if (Result)
	{
		int Deleted = 0, CountIndexes = qSelectIndexes.GetCountResultRows();
		while (qSelectIndexes.Next())
		{
			ISQuery qDeleteIndex;
			qDeleteIndex.SetShowLongQuery(false);
			Result = qDeleteIndex.Execute(QD_INDEX.arg(qSelectIndexes.ReadColumn("indexname").toString()));
			if (Result)
			{
				ISDEBUG_I(QString("Deleted %1 of %2 indexes").arg(++Deleted).arg(CountIndexes));
			}
			else
			{
				ErrorString = qDeleteIndex.GetErrorString();
				break;
			}
		}
	}
	else
	{
		ErrorString = qSelectIndexes.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::foreigns()
{
	ISDEBUG_D("Deleting foreigns...");

	ISQuery qSelectForeigns(QS_FOREIGNS);
	qSelectForeigns.SetShowLongQuery(false);
	bool Result = qSelectForeigns.Execute();
	if (Result)
	{
		int Deleted = 0, CountForeigns = qSelectForeigns.GetCountResultRows();
		while (qSelectForeigns.Next())
		{
			QString ForeignName = qSelectForeigns.ReadColumn("constraint_name").toString();
			QString TableName;

			QStringList StringList = ForeignName.split('_');
			if (StringList[0].length())
			{
				TableName = StringList[0];
			}
			else
			{
				TableName = '_' + StringList[1];
			}


			ISQuery qDeleteForeign;
			qDeleteForeign.SetShowLongQuery(false);
			Result = qDeleteForeign.Execute(QD_FOREIGN.arg(TableName).arg(ForeignName));
			if (Result)
			{
				ISDEBUG_I(QString("Deleted %1 of %2 foreigns").arg(++Deleted).arg(CountForeigns));
			}
			else
			{
				ErrorString = qDeleteForeign.GetErrorString();
				break;
			}
		}
	}
	else
	{
		ErrorString = qSelectForeigns.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldtables()
{
	ISVectorString VectorString;
	std::vector<PMetaTable*> Tables = ISMetaData::Instance().GetTables();
	for (PMetaTable *MetaTable : Tables)
	{
		VectorString.emplace_back(MetaTable->Name.toLower());
	}

	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	bool Result = qSelectTables.Execute();
	if (Result)
	{
		while (qSelectTables.Next())
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString();
			if (!ISAlgorithm::VectorContains(VectorString, TableName))
			{
				if (ISConsole::Question(QString("Remove table \"%1\"?").arg(TableName)))
				{
					ISDEBUG_L(QString("Removing table \"%1\"...").arg(TableName));
					ISQuery qDeleteTable;
					qDeleteTable.SetShowLongQuery(false);
					Result = qDeleteTable.Execute("DROP TABLE public." + TableName);
					if (!Result)
					{
						ErrorString = qDeleteTable.GetErrorString();
					}
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectTables.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldfields()
{
	QMap<QString, ISVectorString> Map;
	std::vector<PMetaTable*> Tables = ISMetaData::Instance().GetTables();
	for (PMetaTable *MetaTable : Tables)
	{
		QString TableName = MetaTable->Name.toLower();
		Map.insert(TableName, ISVectorString());

		for (PMetaField* MetaField : MetaTable->Fields)
		{
			Map[TableName].emplace_back(MetaTable->Alias.toLower() + '_' + MetaField->Name.toLower());
		}
	}

	ISQuery qSelectColumns(QS_COLUMNS);
	qSelectColumns.SetShowLongQuery(false);
	bool Result = qSelectColumns.Execute();
	if (Result)
	{
		while (qSelectColumns.Next())
		{
			QString TableName = qSelectColumns.ReadColumn("table_name").toString();
			QString ColumnName = qSelectColumns.ReadColumn("column_name").toString();
			if (Map.contains(TableName))
			{
				if (!ISAlgorithm::VectorContains(Map.value(TableName), ColumnName))
				{
					if (ISConsole::Question(QString("Remove column \"%1\" in table \"%2\"?").arg(ColumnName).arg(TableName)))
					{
						ISDEBUG_L(QString("Removing column \"%1\"...").arg(ColumnName));
						ISQuery qDeleteField;
						qDeleteField.SetShowLongQuery(false);
						Result = qDeleteField.Execute("ALTER TABLE public." + TableName + " DROP COLUMN " + ColumnName);
						if (!Result)
						{
							ErrorString = qDeleteField.GetErrorString();
						}
					}
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectColumns.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldresources()
{
	QMap<QString, ISVectorString> Map;
	for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
	{
		if (Map.contains(MetaResource->TableName))
		{
			Map[MetaResource->TableName].emplace_back(MetaResource->UID);
		}
		else
		{
			Map.insert(MetaResource->TableName, { MetaResource->UID });
		}
	}

	for (const auto &MapItem : Map.toStdMap())
	{
		QString TableName = MapItem.first;
		ISVectorString UIDs = MapItem.second;
		PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName);

		ISQuery qSelect("SELECT " + MetaTable->Alias + "_uid FROM " + TableName);
		qSelect.SetShowLongQuery(false);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				ISUuid ResourceUID = qSelect.ReadColumn(MetaTable->Alias + "_uid");
				if (!ISAlgorithm::VectorContains<QString>(UIDs, ResourceUID))
				{
					ShowResourceConsole(MetaTable, ResourceUID);
					if (ISConsole::Question(QString("Remove resource \"%1\" in table \"%2\"?").arg(ResourceUID).arg(TableName)))
					{
						ISQuery qDeleteResources("DELETE FROM " + TableName + " WHERE " + MetaTable->Alias + "_uid = :ResourceUID");
						qDeleteResources.SetShowLongQuery(false);
						qDeleteResources.BindValue(":ResourceUID", ResourceUID);
						if (!qDeleteResources.Execute())
						{
							ErrorString = qDeleteResources.GetErrorString();
						}
					}
				}
			}
		}
		else
		{
			ErrorString = qSelect.GetErrorString();
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldsequence()
{
	QString Where;
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		Where += '\'' + MetaTable->Name.toLower() + "_sequence" + "', ";
	}
	Where.chop(2);

	ISQuery qSelect(QS_SEQUENCES.replace(":Where", Where));
	qSelect.SetShowLongQuery(false);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			QString SequenceName = qSelect.ReadColumn("sequence_name").toString();
			if (ISConsole::Question(QString("Delete sequence \"%1\"?").arg(SequenceName)))
			{
				ISQuery qDelete;
				qDelete.SetShowLongQuery(false);
				Result = qDelete.Execute(QD_SEQUENCE.arg(SequenceName));
				if (!Result)
				{
					ErrorString = qDelete.GetErrorString();
				}
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
bool CGConfiguratorDelete::oldindexes()
{
	ISQuery qSelectIndexes(QS_INDEX);
	qSelectIndexes.SetShowLongQuery(false);
	bool Result = qSelectIndexes.Execute();
	if (Result)
	{
		ISVectorString IndexNames;
		for (PMetaIndex *MetaIndex : ISMetaData::Instance().GetIndexes())
		{
			IndexNames.emplace_back(MetaIndex->GetName());
		}

		while (qSelectIndexes.Next())
		{
			QString IndexName = qSelectIndexes.ReadColumn("indexname").toString();
			if (!ISAlgorithm::VectorContains(IndexNames, IndexName))
			{
				if (ISConsole::Question(QString("Delete index %1?").arg(IndexName)))
				{
					ISQuery qDeleteIndex;
					Result = qDeleteIndex.Execute(QD_INDEX.arg(IndexName));
					if (!Result)
					{
						ErrorString = qDeleteIndex.GetErrorString();
						break;
					}
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectIndexes.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldforeigns()
{
	ISQuery qSelectForeigns(QS_FOREIGN);
	qSelectForeigns.SetShowLongQuery(false);
	bool Result = qSelectForeigns.Execute();
	if (Result)
	{
		ISVectorString ForeignNames;
		for (PMetaForeign *MetaForeign : ISMetaData::Instance().GetForeigns())
		{
			ForeignNames.emplace_back(MetaForeign->GetName());
		}

		while (qSelectForeigns.Next())
		{
			QString TableName = qSelectForeigns.ReadColumn("table_name").toString();
			QString ForeignName = qSelectForeigns.ReadColumn("constraint_name").toString();
			if (!ISAlgorithm::VectorContains(ForeignNames, ForeignName))
			{
				if (ISConsole::Question(QString("Delete foreign %1?").arg(ForeignName)))
				{
					ISQuery qDeleteForeign;
					qDeleteForeign.SetShowLongQuery(false);
					Result = qDeleteForeign.Execute(QD_FOREIGN.arg(TableName).arg(ForeignName));
					if (!Result)
					{
						ErrorString = qDeleteForeign.GetErrorString();
						break;
					}
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectForeigns.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::ShowResourceConsole(PMetaTable *MetaTable, const ISUuid &ResourceUID)
{
	ISDEBUG();
	ISDEBUG_L("Table name: " + MetaTable->Name);
	ISQuery qSelect("SELECT * FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_uid = :ResourceUID");
	qSelect.BindValue(":ResourceUID", ResourceUID);
	if (qSelect.ExecuteFirst())
	{
		QSqlRecord SqlRecord = qSelect.GetRecord();
		for (int i = 0; i < SqlRecord.count(); ++i)
		{
			ISDEBUG_L(QString("%1: %2").arg(SqlRecord.field(i).name()).arg(SqlRecord.value(i).toString()));
		}
	}
}
//-----------------------------------------------------------------------------
