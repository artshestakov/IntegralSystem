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
void CGConfiguratorShow::obsoletetables()
{
	ISLOGGER_UNKNOWN("Searching not needed tables...");

	int FoundedTables = 0;

	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	if (qSelectTables.Execute())
	{
		ISLOGGER_UNKNOWN("Tables:");
		while (qSelectTables.Next()) //Обход таблиц базы
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString().toLower();

			PMetaClassTable *MetaTable = FoundTable(TableName);
			if (!MetaTable)
			{
				ISLOGGER_UNKNOWN(TableName);
				++FoundedTables;
			}
		}
	}

	ISLOGGER_UNKNOWN(QString("Founded tables: %1").arg(FoundedTables));
	ISLOGGER_EMPTY();
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::obsoletefields()
{
	ISLOGGER_UNKNOWN("Searching not needed fields...");

	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	if (qSelectTables.Execute())
	{
		while (qSelectTables.Next()) //Обход таблиц базы
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString().toLower();

			PMetaClassTable *MetaTable = FoundTable(TableName);
			if (MetaTable)
			{
				ISQuery qSelectColumns(QS_COLUMNS);
				qSelectColumns.SetShowLongQuery(false);
				qSelectColumns.BindValue(":TableName", TableName);
				if (qSelectColumns.Execute())
				{
					while (qSelectColumns.Next()) //Обход полей таблицы
					{
						QString ColumnName = qSelectColumns.ReadColumn("column_name").toString().toLower();

						PMetaClassField *MetaField = FoundField(MetaTable, ColumnName);
						if (!MetaField)
						{
							ISLOGGER_UNKNOWN(TableName + ": " + ColumnName);
						}
					}
				}
			}
		}
	}
	ISLOGGER_EMPTY();
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::obsoleteresources()
{
	ISLOGGER_UNKNOWN("Searching not needed resources...");

	std::map<QString, ISVectorString> Map, MapOutput;
	for (PMetaClassResource *MetaResource : ISMetaData::GetInstanse().GetResources())
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
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::obsoletesequence()
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
	if (qSelectSequences.Execute())
	{
		while (qSelectSequences.Next())
		{
			QString SequenceName = qSelectSequences.ReadColumn("sequence_name").toString();
			ISLOGGER_UNKNOWN(SequenceName);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::config()
{
	QFile FileConfig(ISDefines::Core::PATH_CONFIG_FILE);
	if (FileConfig.exists())
	{
		if (FileConfig.open(QIODevice::ReadOnly))
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
			ISLOGGER_UNKNOWN(QString("Not open file config: %1").arg(FileConfig.errorString()));
		}
	}
	else
	{
		ISLOGGER_UNKNOWN(QString("Not exist file config: %1").arg(FileConfig.fileName()));
	}
}
//-----------------------------------------------------------------------------
PMetaClassTable* CGConfiguratorShow::FoundTable(const QString &TableName)
{
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().size(); ++i)
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		if (MetaTable->Name.toLower() == TableName)
		{
			return MetaTable;
		}
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaClassField* CGConfiguratorShow::FoundField(PMetaClassTable *MetaTable, const QString &ColumnName)
{
	for (int i = 0; i < MetaTable->AllFields.count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->AllFields[i];
		if (QString(MetaTable->Alias + '_' + MetaField->Name).toLower() == ColumnName)
		{
			return MetaField;
		}
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::databasesize()
{
	ISLOGGER_UNKNOWN(ISDatabase::GetInstance().GetCurrentDatabaseSize());
}
//-----------------------------------------------------------------------------
