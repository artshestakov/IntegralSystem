#include "CGConfiguratorShow.h"
#include "ISConfig.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static QString QS_TABLES = PREPARE_QUERY("SELECT tablename "
										 "FROM pg_tables "
										 "WHERE schemaname = current_schema() "
										 "ORDER BY tablename");
//-----------------------------------------------------------------------------
static QString QS_COLUMNS = PREPARE_QUERY("SELECT column_name "
										  "FROM information_schema.columns "
										  "WHERE table_catalog = current_database() "
										  "AND table_schema = current_schema() "
										  "AND table_name = :TableName "
										  "ORDER BY column_name");
//-----------------------------------------------------------------------------
static QString QS_SEQUENCES = PREPARE_QUERY("SELECT sequence_name "
											"FROM information_schema.sequences "
											"WHERE sequence_catalog = current_database() "
											"AND sequence_name NOT IN(:Where) "
											"ORDER BY sequence_name");
//-----------------------------------------------------------------------------
static QString QS_INDEX = PREPARE_QUERY("SELECT indexname "
										"FROM pg_indexes "
										"JOIN pg_class c ON c.relname = indexname "
										"JOIN pg_index ON indexrelid = c.oid "
										"WHERE schemaname = current_schema() "
										"AND right(indexname, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static QString QS_FOREIGN = PREPARE_QUERY("SELECT constraint_name "
										  "FROM information_schema.table_constraints "
										  "WHERE constraint_type = 'FOREIGN KEY' "
										  "AND table_schema = current_schema() "
										  "ORDER BY constraint_name");
//-----------------------------------------------------------------------------
static QString QS_INFO = PREPARE_QUERY("SELECT "
									   "(SELECT pg_size_pretty(pg_database_size(current_database()))) AS \"database_size\", "
									   "(SELECT pg_catalog.pg_get_userbyid(datdba) AS \"database_owner\" FROM pg_catalog.pg_database WHERE datname = current_database()), "
									   "(SELECT pg_encoding_to_char(encoding) AS \"database_encoding\" FROM pg_database WHERE datname = current_database()), "
									   "(SELECT now() - pg_postmaster_start_time() AS \"database_uptime\"), "
									   "(SELECT pg_backend_pid() AS pid) AS \"database_backend_pid\", "
									   "(SELECT version()) AS \"database_version\", "
									   "(SELECT setting AS \"database_cluster_path\" FROM pg_settings WHERE name = 'data_directory'), "
									   "(SELECT COUNT(*) AS \"table_count\" FROM information_schema.tables WHERE table_catalog = current_database() AND table_schema = current_schema()), "
									   "(SELECT COUNT(*) AS \"field_count\" FROM information_schema.columns WHERE table_catalog = current_database() AND table_schema = current_schema()), "
									   "(SELECT COUNT(*) AS \"sequence_count\" FROM information_schema.sequences WHERE sequence_catalog = current_database() AND sequence_schema = current_schema()), "
									   "(SELECT COUNT(*) AS \"index_count\" FROM pg_indexes WHERE schemaname = current_schema()), "
									   "(SELECT COUNT(*) AS \"foreign_count\" FROM information_schema.constraint_table_usage WHERE constraint_catalog = current_database() AND constraint_schema = current_schema())");
//-----------------------------------------------------------------------------
CGConfiguratorShow::CGConfiguratorShow() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorShow::~CGConfiguratorShow()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldobjects()
{
	int Tables = 0, Fields = 0, Resources = 0, Sequences = 0, Indexes = 0, Foreigns = 0;
	bool Result = oldtables(Tables);
	if (Result)
	{
		Result = oldfields(Fields);
	}

	if (Result)
	{
		Result = oldresources(Resources);
	}

	if (Result)
	{
		Result = oldsequence(Sequences);
	}

	if (Result)
	{
		Result = oldindexes(Indexes);
	}

	if (Result)
	{
		Result = oldforeigns(Foreigns);
	}

	ISDEBUG_L(QString("Tables: %1 Fields: %2 Resources: %3 Sequences: %4 Indexes: %5 Foreigns: %6")
		.arg(Tables).arg(Fields).arg(Resources).arg(Sequences).arg(Indexes).arg(Foreigns));
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::databaseinfo()
{
	ISQuery qSelectInfo(QS_INFO);
	qSelectInfo.SetShowLongQuery(false);
	bool Result = qSelectInfo.ExecuteFirst();
	if (Result)
	{
		ISDEBUG_L("Size:\t\t" + qSelectInfo.ReadColumn("database_size").toString());
		ISDEBUG_L("Owner:\t\t" + qSelectInfo.ReadColumn("database_owner").toString());
		ISDEBUG_L("Encoding:\t" + qSelectInfo.ReadColumn("database_encoding").toString());
		ISDEBUG_L("Uptime:\t\t" + qSelectInfo.ReadColumn("database_uptime").toString());
		ISDEBUG_L("Backend PID:\t" + qSelectInfo.ReadColumn("database_backend_pid").toString());
		ISDEBUG_L("Version:\t" + qSelectInfo.ReadColumn("database_version").toString());
		ISDEBUG_L("Cluster path:\t" + qSelectInfo.ReadColumn("database_cluster_path").toString());
		ISDEBUG_L("Count tables:\t" + qSelectInfo.ReadColumn("table_count").toString());
		ISDEBUG_L("Count fields:\t" + qSelectInfo.ReadColumn("field_count").toString());
		ISDEBUG_L("Count sequence:\t" + qSelectInfo.ReadColumn("sequence_count").toString());
		ISDEBUG_L("Count foreigns:\t" + qSelectInfo.ReadColumn("foreign_count").toString());
		
		//������� ������ ��� ������� ���������� �����
		QString SqlQueryCount = "WITH r AS(\n";
		for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
		{
			SqlQueryCount += QString("SELECT COUNT(*) FROM %1\nUNION\n").arg(MetaTable->Name);
		}
		SqlQueryCount.chop(6);
		SqlQueryCount += ") SELECT sum(count) FROM r";

		//��������� ������
		ISQuery qSelectCount(SqlQueryCount);
		qSelectCount.SetShowLongQuery(false);
		if (qSelectCount.ExecuteFirst())
		{
			ISDEBUG_L("Count records:\t" + qSelectCount.ReadColumn("sum").toString());
		}
		else
		{
			ISDEBUG_L("Error getting count records: " + qSelectCount.GetErrorString());
		}
	}
	else
	{
		ISDEBUG_L(qSelectInfo.GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldtables(int &Count)
{
	ISDEBUG_L("Search tables...");
	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	bool Result = qSelectTables.Execute();
	if (Result)
	{
		while (qSelectTables.Next()) //����� ������ ����
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString().toLower();
			PMetaTable *MetaTable = FoundTable(TableName);
			if (!MetaTable)
			{
				ISDEBUG_L(TableName);
				++Count;
			}
		}
	}
	else
	{
		ErrorString = qSelectTables.GetErrorString();
	}
	ISDEBUG();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldfields(int &Count)
{
	ISDEBUG_L("Search fields...");
	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	bool Result = qSelectTables.Execute();
	if (Result)
	{
		while (qSelectTables.Next()) //����� ������ ����
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString().toLower();
			PMetaTable *MetaTable = FoundTable(TableName);
			if (MetaTable)
			{
				ISQuery qSelectColumns(QS_COLUMNS);
				qSelectColumns.SetShowLongQuery(false);
				qSelectColumns.BindValue(":TableName", TableName);
				Result = qSelectColumns.Execute();
				if (Result)
				{
					while (qSelectColumns.Next()) //����� ����� �������
					{
						QString ColumnName = qSelectColumns.ReadColumn("column_name").toString().toLower();
						PMetaField *MetaField = FoundField(MetaTable, ColumnName);
						if (!MetaField)
						{
							ISDEBUG_L(TableName + ": " + ColumnName);
							++Count;
						}
					}
				}
				else
				{
					ErrorString = qSelectColumns.GetErrorString();
					break;
				}
			}
		}
	}
	else
	{
		ErrorString = qSelectTables.GetErrorString();
	}
	ISDEBUG();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldresources(int &Count)
{
	ISDEBUG_L("Search resources...");
	std::map<QString, ISVectorString> Map, MapOutput;
	for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
	{
		QString TableName = MetaResource->TableName;
		QString ResourceUID = MetaResource->UID.toLower();
		if (!Map.count(TableName))
		{
			Map.emplace(TableName, ISVectorString());
		}
		Map[TableName].emplace_back(ResourceUID);
	}
	for (const auto &MapItem : Map)
	{
		QString TableName = MapItem.first;
		ISVectorString Vector = MapItem.second;

		QString SqlText = QString("SELECT %1_uid FROM %2 WHERE %1_uid NOT IN(%3)").arg(ISMetaData::Instance().GetMetaTable(TableName)->Alias).arg(TableName);
		QString NotIN;
		for (const QString &String : Vector)
		{
			NotIN += '\'' + String + "', ";
		}
		NotIN.chop(2);
		SqlText = SqlText.arg(NotIN);

		ISQuery qSelect(SqlText);
		qSelect.SetShowLongQuery(false);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				if (!MapOutput.count(TableName))
				{
					MapOutput.emplace(TableName, ISVectorString());
				}
				MapOutput[TableName].emplace_back(qSelect.ReadColumn(0).toString());
			}
		}
		else
		{
			ErrorString = qSelect.GetErrorString();
			return false;
		}
	}
	for (const auto &OutputItem : MapOutput)
	{
		for (const QString &String : OutputItem.second)
		{
			ISDEBUG_L(OutputItem.first + ": " + String);
			++Count;
		}
	}
	ISDEBUG();
	return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldsequence(int &Count)
{
	ISDEBUG_L("Search sequences...");
	QString Where;
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		Where += '\'' + MetaTable->Name.toLower() + "_sequence" + "', ";
	}
	Where.chop(2);
	ISQuery qSelectSequences(QS_SEQUENCES.replace(":Where", Where));
	qSelectSequences.SetShowLongQuery(false);
	bool Result = qSelectSequences.Execute();
	if (Result)
	{
		while (qSelectSequences.Next())
		{
			ISDEBUG_L(qSelectSequences.ReadColumn("sequence_name").toString());
			++Count;
		}
	}
	else
	{
		ErrorString = qSelectSequences.GetErrorString();
	}
	ISDEBUG();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldindexes(int &Count)
{
	ISDEBUG_L("Search indexes...");
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
				ISDEBUG_L(IndexName);
				++Count;
			}
		}
	}
	else
	{
		ErrorString = qSelectIndexes.GetErrorString();
	}
	ISDEBUG();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldforeigns(int &Count)
{
	ISDEBUG_L("Search foreigns...");
	ISQuery qSelectForeigns(QS_FOREIGN);
	qSelectForeigns.SetShowLongQuery(false);
	bool Result = qSelectForeigns.Execute();
	if (Result)
	{
		ISVectorString Foreigns;
		for (PMetaForeign *MetaForeign : ISMetaData::Instance().GetForeigns())
		{
			Foreigns.emplace_back(MetaForeign->GetName());
		}

		while (qSelectForeigns.Next())
		{
			QString ForeignName = qSelectForeigns.ReadColumn("constraint_name").toString();
			if (!ISAlgorithm::VectorContains(Foreigns, ForeignName))
			{
				ISDEBUG_L(ForeignName);
				++Count;
			}
		}
	}
	else
	{
		ErrorString = qSelectForeigns.GetErrorString();
	}
	ISDEBUG();
	return Result;
}
//-----------------------------------------------------------------------------
PMetaTable* CGConfiguratorShow::FoundTable(const QString &TableName)
{
    for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		if (MetaTable->Name.toLower() == TableName)
		{
			return MetaTable;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaField* CGConfiguratorShow::FoundField(PMetaTable *MetaTable, const QString &ColumnName)
{
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (QString(MetaTable->Alias + '_' + MetaField->Name).toLower() == ColumnName)
		{
			return MetaField;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
