#include "CGConfiguratorDelete.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
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
static QString QD_SYSTEMS = PREPARE_QUERY("DELETE FROM _systems");
//-----------------------------------------------------------------------------
static QString QD_SUB_SYSTEMS = PREPARE_QUERY("DELETE FROM _subsystems");
//-----------------------------------------------------------------------------
static QString QS_TABLES = PREPARE_QUERY("SELECT table_name "
										 "FROM information_schema.tables "
										 "WHERE table_catalog = current_database() "
										 "AND table_schema = 'public' "
										 "AND table_name != '_cdr' "
										 "ORDER BY table_name");
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
CGConfiguratorDelete::CGConfiguratorDelete() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorDelete::~CGConfiguratorDelete()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::indexes()
{
	ISQuery qSelectIndexes(QS_INDEXES);
	bool Result = qSelectIndexes.Execute();
	if (Result)
	{
		int Deleted = 0, CountIndexes = qSelectIndexes.GetCountResultRows();
		while (qSelectIndexes.Next())
		{
			ISQuery qDeleteIndex;
			Result = qDeleteIndex.Execute(QD_INDEX.arg(qSelectIndexes.ReadColumn("indexname").toString()));
			if (Result)
			{
				ISLOGGER_I(QString("Deleted %1 of %2 indexes").arg(++Deleted).arg(CountIndexes));
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
	ISLOGGER_D("Deleting foreigns...");

	ISQuery qSelectForeigns(QS_FOREIGNS);
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
			Result = qDeleteForeign.Execute(QD_FOREIGN.arg(TableName).arg(ForeignName));
			if (Result)
			{
				ISLOGGER_I(QString("Deleted %1 of %2 foreigns").arg(++Deleted).arg(CountForeigns));
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
bool CGConfiguratorDelete::systems()
{
	ISLOGGER_D("Deleting systems...");

	ISQuery qDelete(QD_SYSTEMS);
	bool Result = qDelete.Execute();
	if (Result)
	{
		ISLOGGER_I("Deleting all systems done");
	}
	else
	{
		ErrorString = qDelete.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::subsystems()
{
	ISLOGGER_D("Deleting subsystems...");

	ISQuery qDelete(QD_SUB_SYSTEMS);
	bool Result = qDelete.Execute();
	if (Result)
	{
		ISLOGGER_I("Deleting all subsystems done");
	}
	else
	{
		ErrorString = qDelete.GetErrorString();
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

	int Removed = 0, Skipped = 0;

	ISQuery qSelectTables(QS_TABLES);
	bool Result = qSelectTables.Execute();
	if (Result)
	{
		while (qSelectTables.Next())
		{
			QString TableName = qSelectTables.ReadColumn("table_name").toString();
			if (!ISAlgorithm::VectorContains(VectorString, TableName))
			{
				if (ISConsole::Question(QString("Remove table \"%1\"?").arg(TableName)))
				{
					ISLOGGER_L(QString("Removing table \"%1\"...").arg(TableName));
					ISQuery qDeleteTable;
					Result = qDeleteTable.Execute("DROP TABLE public." + TableName);
					if (Result)
					{
						++Removed;
						ISLOGGER_L("Removed table");
					}
					else
					{
						ErrorString = qDeleteTable.GetErrorString();
					}
				}
				else
				{
					++Skipped;
				}
			}
		}
	}
	ISLOGGER_L(Removed == Skipped ? "Not found obsolete tables" : QString("Removed tables: %1. Skipped tables: %2").arg(Removed).arg(Skipped));
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

	int Removed = 0, Skipped = 0;

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
						ISLOGGER_L(QString("Removing column \"%1\"...").arg(ColumnName));
						ISQuery qDeleteField;
						Result = qDeleteField.Execute("ALTER TABLE public." + TableName + " DROP COLUMN " + ColumnName);
						if (Result)
						{
							++Removed;
							ISLOGGER_L("Removed column");
						}
						else
						{
							ErrorString = qDeleteField.GetErrorString();
						}
					}
					else
					{
						++Skipped;
					}
				}
			}
		}
	}
	ISLOGGER_L(Removed == Skipped ? "Not found obsolete fields" : QString("Removed columns: %1. Skipped columns: %2").arg(Removed).arg(Skipped));
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

	int Removed = 0, Skipped = 0;
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
						if (qDeleteResources.Execute())
						{
							++Removed;
						}
						else
						{
							ISLOGGER_E("Error delete resource: " + qDeleteResources.GetErrorString());
						}
					}
					else
					{
						++Skipped;
					}
				}
			}
		}
		else
		{
			ISLOGGER_E(qSelect.GetErrorString());
		}
	}

	ISLOGGER_N();
	Removed == Skipped ?
		ISLOGGER_L("Not found obsolete resources") :
		ISLOGGER_L(QString("Removed resources: %1. Skipped resources: %2").arg(Removed).arg(Skipped));
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
	bool Result = qSelect.Execute();
	if (Result)
	{
		int Removed = 0, Skipped = 0;
		while (qSelect.Next())
		{
			QString SequenceName = qSelect.ReadColumn("sequence_name").toString();
			if (ISConsole::Question(QString("Delete sequence \"%1\"?").arg(SequenceName)))
			{
				ISQuery qDelete;
				Result = qDelete.Execute(QD_SEQUENCE.arg(SequenceName));
				if (Result)
				{
					++Removed;
				}
				else
				{
					ISLOGGER_E(qDelete.GetErrorString());
				}
			}
			else
			{
				++Skipped;
			}
		}
		ISLOGGER_L(Removed == Skipped ?
			"\nNot found old sequence" :
			QString("\nRemoved resources: %1. Skipped resources: %2").arg(Removed).arg(Skipped));
	}
	else
	{
		ISLOGGER_E(qSelect.GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::ShowResourceConsole(PMetaTable *MetaTable, const ISUuid &ResourceUID)
{
	ISLOGGER_N();
	ISLOGGER_L("Table name: " + MetaTable->Name);
	ISQuery qSelect("SELECT * FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_uid = :ResourceUID");
	qSelect.BindValue(":ResourceUID", ResourceUID);
	if (qSelect.ExecuteFirst())
	{
		QSqlRecord SqlRecord = qSelect.GetRecord();
		for (int i = 0; i < SqlRecord.count(); ++i)
		{
			ISLOGGER_L(QString("%1: %2").arg(SqlRecord.field(i).name()).arg(SqlRecord.value(i).toString()));
		}
	}
}
//-----------------------------------------------------------------------------
