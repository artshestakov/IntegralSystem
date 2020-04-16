#include "CGConfiguratorShow.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISSystem.h"
#include "ISDatabase.h"
#include "ISDefinesCore.h"
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
CGConfiguratorShow::CGConfiguratorShow() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorShow::~CGConfiguratorShow()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::obsoletetables()
{
	ISLOGGER_UNKNOWN("Searching not needed tables...");

	int FoundedTables = 0;

	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	bool Result = qSelectTables.Execute();
	if (Result)
	{
		ISLOGGER_UNKNOWN("Tables:");
		while (qSelectTables.Next()) //Обход таблиц базы
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString().toLower();
			PMetaTable *MetaTable = FoundTable(TableName);
			if (!MetaTable)
			{
				ISLOGGER_UNKNOWN(TableName);
				++FoundedTables;
			}
		}
	}
	else
	{
		ErrorString = qSelectTables.GetErrorString();
	}

	ISLOGGER_UNKNOWN(QString("Founded tables: %1").arg(FoundedTables));
	ISLOGGER_EMPTY();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::obsoletefields()
{
	ISLOGGER_UNKNOWN("Searching not needed fields...");

	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	bool Result = qSelectTables.Execute();
	if (Result)
	{
		while (qSelectTables.Next()) //Обход таблиц базы
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
					while (qSelectColumns.Next()) //Обход полей таблицы
					{
						QString ColumnName = qSelectColumns.ReadColumn("column_name").toString().toLower();
						PMetaField *MetaField = FoundField(MetaTable, ColumnName);
						if (!MetaField)
						{
							ISLOGGER_UNKNOWN(TableName + ": " + ColumnName);
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

	ISLOGGER_EMPTY();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::obsoleteresources()
{
	ISLOGGER_UNKNOWN("Searching not needed resources...");

	std::map<QString, ISVectorString> Map, MapOutput;
	for (PMetaResource *MetaResource : ISMetaData::GetInstanse().GetResources())
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

		QString SqlText = QString("SELECT %1_uid FROM %2 WHERE %1_uid NOT IN(%3)").arg(ISMetaData::GetInstanse().GetMetaTable(TableName)->Alias).arg(TableName);
		QString NotIN;
		for (const QString &String : Vector)
		{
			NotIN += '\'' + String + "', ";
		}
		ISSystem::RemoveLastSymbolFromString(NotIN, 2);
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
	ISLOGGER_EMPTY();
	for (const auto &OutputItem : MapOutput)
	{
		QString TableName = OutputItem.first;
		ISVectorString Vector = OutputItem.second;

		ISLOGGER_UNKNOWN(TableName + ':');
		for (const QString &String : Vector)
		{
			ISLOGGER_UNKNOWN(String);
		}
		ISLOGGER_EMPTY();
	}
	ISLOGGER_EMPTY();
	return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::obsoletesequence()
{
	ISLOGGER_UNKNOWN("Searching not needed sequences...");

	QString Where;
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().size(); ++i)
	{
		QString TableName = ISMetaData::GetInstanse().GetTables()[i]->Name.toLower();
		QString SequnceName = TableName + "_sequence";
		Where += '\'' + SequnceName + "', ";
	}

	ISSystem::RemoveLastSymbolFromString(Where, 2);

	ISQuery qSelectSequences(QS_SEQUENCES.replace(":Where", Where));
	qSelectSequences.SetShowLongQuery(false);
	bool Result = qSelectSequences.Execute();
	if (Result)
	{
		while (qSelectSequences.Next())
		{
			ISLOGGER_UNKNOWN(qSelectSequences.ReadColumn("sequence_name").toString());
		}
	}
	else
	{
		ErrorString = qSelectSequences.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::config()
{
	QFile FileConfig(ISDefines::Core::PATH_CONFIG_FILE);
	bool Result = FileConfig.exists();
	if (Result)
	{
		Result = FileConfig.open(QIODevice::ReadOnly);
		if (Result)
		{
			QStringList StringList = QString(FileConfig.readAll()).split("\n");
			for (const QString &String : StringList)
			{
				ISLOGGER_UNKNOWN(String);
			}
			FileConfig.close();
		}
		else
		{
			ErrorString = QString("Not open file config: %1").arg(FileConfig.errorString());
		}
	}
	else
	{
		ErrorString = QString("Not exist file config: %1").arg(FileConfig.fileName());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::databasesize()
{
	ISLOGGER_UNKNOWN(ISDatabase::Instance().GetCurrentDatabaseSize());
	return true;
}
//-----------------------------------------------------------------------------
PMetaTable* CGConfiguratorShow::FoundTable(const QString &TableName)
{
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().size(); ++i)
	{
		PMetaTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
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
	for (PMetaField *MetaField : MetaTable->AllFields)
	{
		if (QString(MetaTable->Alias + '_' + MetaField->Name).toLower() == ColumnName)
		{
			return MetaField;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
