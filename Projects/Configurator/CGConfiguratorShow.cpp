#include "StdAfx.h"
#include "CGConfiguratorShow.h"
#include "ISDefines.h"
#include "ISConfig.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISSystem.h"
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
											"ORDER BY sequence_name")
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
	ISDebug::ShowString("Searching not needed tables...");

	int FoundedTables = 0;

	ISQuery qSelectTables(QS_TABLES);
	qSelectTables.SetShowLongQuery(false);
	if (qSelectTables.Execute())
	{
		ISDebug::ShowString("Tables:");
		while (qSelectTables.Next()) //Обход таблиц базы
		{
			QString TableName = qSelectTables.ReadColumn("tablename").toString().toLower();

			PMetaClassTable *MetaTable = FoundTable(TableName);
			if (!MetaTable)
			{
				ISDebug::ShowString(TableName);
				FoundedTables++;
			}
		}
	}

	ISDebug::ShowString(QString("Founded tables: %1").arg(FoundedTables));
	ISDebug::ShowEmptyString(true);
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::obsoletefields()
{
	ISDebug::ShowString("Searching not needed fields...");

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
							ISDebug::ShowString(TableName + ": " + ColumnName);
						}
					}
				}
			}
		}
	}

	ISDebug::ShowEmptyString(true);
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::obsoleteresources()
{
	ISDebug::ShowString("Searching not needed resources...");

	QMap<QString, QVectorString*> Map;
	QMap <QString, QVectorString*> MapOutput;

	for (int i = 0; i < ISMetaData::GetInstanse().GetResources().count(); ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);

		QString TableName = MetaResource->GetTableName();
		QString ResourceUID = MetaResource->GetUID().toLower();

		if (!Map.contains(TableName))
		{
			Map.insert(TableName, new QVectorString());
		}

		Map.value(TableName)->append(ResourceUID);
	}

	for (const auto &MapItem : Map.toStdMap())
	{
		QString TableName = MapItem.first;
		QVectorString *Vector = MapItem.second;

		QString SqlText = "SELECT %1_uid FROM %2 WHERE %1_uid NOT IN(%3)";
		SqlText = SqlText.arg(ISMetaData::GetInstanse().GetMetaTable(TableName)->GetAlias());
		SqlText = SqlText.arg(TableName);

		QString NotIN;
		for (int i = 0; i < Vector->count(); ++i)
		{
			NotIN += "'" + Vector->at(i) + "', ";
		}

		ISSystem::RemoveLastSymbolFromString(NotIN, 2);
		SqlText = SqlText.arg(NotIN);

		ISQuery qSelect(SqlText);
		qSelect.SetShowLongQuery(false);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				ISUuid UID = qSelect.ReadColumn(0).toString();

				if (!MapOutput.contains(TableName))
				{
					MapOutput.insert(TableName, new QVectorString());
				}

				MapOutput.value(TableName)->append(UID);
			}
		}
	}

	ISDebug::ShowEmptyString(true);

	for (const auto &OutputItem : MapOutput.toStdMap())
	{
		QString TableName = OutputItem.first;
		QVectorString *Vector = OutputItem.second;

		ISDebug::ShowString(TableName + ":");

		for (int i = 0; i < Vector->count(); ++i)
		{
			ISDebug::ShowString(Vector->at(i));
		}

		ISDebug::ShowEmptyString(true);
	}

	ISDebug::ShowEmptyString(true);
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::obsoletesequence()
{
	ISDebug::ShowString("Searching not needed sequences...");

	QString Where;
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().count(); ++i)
	{
		QString TableName = ISMetaData::GetInstanse().GetTables().at(i)->GetName().toLower();
		QString SequnceName = TableName + "_sequence";
		Where += "'" + SequnceName + "', ";
	}

	ISSystem::RemoveLastSymbolFromString(Where, 2);

	ISQuery qSelectSequences(QS_SEQUENCES.replace(":Where", Where));
	qSelectSequences.SetShowLongQuery(false);
	if (qSelectSequences.Execute())
	{
		while (qSelectSequences.Next())
		{
			QString SequenceName = qSelectSequences.ReadColumn("sequence_name").toString();
			ISDebug::ShowString(SequenceName);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::config()
{
	QFile FileConfig(APPLICATION_DIR_PATH + "/Config.ini");
	if (FileConfig.exists())
	{
		if (FileConfig.open(QIODevice::ReadOnly))
		{
			QStringList StringList = QString(FileConfig.readAll()).split("\n");
			for (const QString &String : StringList)
			{
				ISDebug::ShowString(String);
			}

			FileConfig.close();
		}
		else
		{
			ISDebug::ShowString(QString("Not open file config: %1").arg(FileConfig.errorString()));
		}
	}
	else
	{
		ISDebug::ShowString(QString("Not exist file config: %1").arg(FileConfig.fileName()));
	}
}
//-----------------------------------------------------------------------------
PMetaClassTable* CGConfiguratorShow::FoundTable(const QString &TableName)
{
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().count(); ++i)
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		if (MetaTable->GetName().toLower() == TableName)
		{
			return MetaTable;
		}
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
PMetaClassField* CGConfiguratorShow::FoundField(PMetaClassTable *MetaTable, const QString &ColumnName)
{
	for (int i = 0; i < MetaTable->GetAllFields().count(); ++i)
	{
		PMetaClassField *MetaField = MetaTable->GetAllFields().at(i);
		if (QString(MetaTable->GetAlias() + '_' + MetaField->GetName()).toLower() == ColumnName)
		{
			return MetaField;
		}
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
void CGConfiguratorShow::databasesize()
{
	ISDebug::ShowString(ISDatabase::GetInstance().GetCurrentDatabaseSize());
}
//-----------------------------------------------------------------------------
