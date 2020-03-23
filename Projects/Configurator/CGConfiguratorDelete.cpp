#include "CGConfiguratorDelete.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT indexname "
										  "FROM pg_indexes "
										  "WHERE schemaname = current_schema()");
//-----------------------------------------------------------------------------
static QString QD_INDEX = "DROP INDEX public.%1 CASCADE";
//-----------------------------------------------------------------------------
static QString QS_FOREIGNS = PREPARE_QUERY("SELECT constraint_name "
										   "FROM information_schema.constraint_table_usage "
										   "WHERE table_catalog = current_database() "
										   "AND table_schema = current_schema()");
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
				++Deleted;
			}
			else
			{
				ErrorString = qDeleteIndex.GetErrorString();
				break;
			}
		}
		ISLOGGER_INFO(QString("Deleted %1 of %2 indexes").arg(Deleted).arg(CountIndexes));
	}
	else
	{
		ErrorString = qSelectIndexes.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::foreigns()
{
	ISLOGGER_DEBUG("Deleting foreigns...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qSelectForeigns(QS_FOREIGNS);
	if (qSelectForeigns.Execute())
	{
		int Deleted = 0;
		int CountForeigns = qSelectForeigns.GetCountResultRows();

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
			if (qDeleteForeign.Execute(QD_FOREIGN.arg(TableName).arg(ForeignName)))
			{
				++Deleted;
			}
		}

		ISLOGGER_INFO("Deleted " + QString::number(Deleted) + " of " + QString::number(CountForeigns) + " foreigns");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::systems()
{
	ISLOGGER_DEBUG("Deleting systems...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qDelete(QD_SYSTEMS);
	if (qDelete.Execute())
	{
		ISLOGGER_INFO("Deleting all systems done");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::subsystems()
{
	ISLOGGER_DEBUG("Deleting subsystems...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qDelete(QD_SUB_SYSTEMS);
	if (qDelete.Execute())
	{
		ISLOGGER_INFO("Deleting all subsystems done");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::tables()
{
	ISVectorString VectorString;
	std::vector<PMetaTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaTable *MetaTable : Tables)
	{
		VectorString.emplace_back(MetaTable->Name.toLower());
	}

	int Removed = 0;
	int Skipped = 0;

	ISQuery qSelectTables(QS_TABLES);
	if (qSelectTables.Execute())
	{
		while (qSelectTables.Next()) //Обход таблиц базы данных
		{
			QString TableName = qSelectTables.ReadColumn("table_name").toString();
			if (!VectorContains(VectorString, TableName)) //Если таблица из базы данных отсутствует в мета-данных
			{
				if (ISConsole::Question(QString("Remove table \"%1\"?").arg(TableName))) //Удаление таблицы
				{
					ISQuery qDeleteTable;
					ISLOGGER_UNKNOWN(QString("Removing table \"%1\"...").arg(TableName));
					bool Executed = qDeleteTable.Execute("DROP TABLE public." + TableName);
					if (Executed)
					{
						++Removed;
						ISLOGGER_UNKNOWN("Removed table");
					}
				}
				else //Пропустить удаление таблицы
				{
					++Skipped;
				}
			}
		}
	}

	if (Removed == Skipped) //Таблицы для удаления не найдены
	{
		ISLOGGER_UNKNOWN("Not found obsolete tables");
	}
	else
	{
		ISLOGGER_UNKNOWN(QString("Removed tables: %1. Skipped tables: %2").arg(Removed).arg(Skipped));
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::fields()
{
	QMap<QString, ISVectorString> Map;
	std::vector<PMetaTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaTable *MetaTable : Tables)
	{
		QString TableName = MetaTable->Name.toLower();
		Map.insert(TableName, ISVectorString());

		for (PMetaField* MetaField : MetaTable->AllFields)
		{
			Map[TableName].emplace_back(MetaTable->Alias.toLower() + '_' + MetaField->Name.toLower());
		}
	}

	int Removed = 0, Skipped = 0;

	ISQuery qSelectColumns(QS_COLUMNS);
	qSelectColumns.SetShowLongQuery(false);
	if (qSelectColumns.Execute())
	{
		while (qSelectColumns.Next()) //Обход всех полей базы данных
		{
			QString TableName = qSelectColumns.ReadColumn("table_name").toString();
			QString ColumnName = qSelectColumns.ReadColumn("column_name").toString();

			if (Map.contains(TableName))
			{
				if (!VectorContains(Map.value(TableName), ColumnName)) //Если поле из базы данных отсутствует в мета-данных
				{
					if (ISConsole::Question(QString("Remove column \"%1\" in table \"%2\"?").arg(ColumnName).arg(TableName))) //Удаление поля
					{
						ISQuery qDeleteField;
						ISLOGGER_UNKNOWN(QString("Removing column \"%1\"...").arg(ColumnName));
						bool Executed = qDeleteField.Execute("ALTER TABLE public." + TableName + " DROP COLUMN " + ColumnName);
						if (Executed)
						{
							++Removed;
							ISLOGGER_UNKNOWN("Removed column");
						}
					}
					else //Пропустить удаление поля
					{
						++Skipped;
					}
				}
			}
		}
	}

	if (Removed == Skipped) //Поля для удаления не найдены
	{
		ISLOGGER_UNKNOWN("Not found obsolete fields");
	}
	else
	{
		ISLOGGER_UNKNOWN(QString("Removed columns: %1. Skipped columns: %2").arg(Removed).arg(Skipped));
	}
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::resources()
{
	QMap<QString, ISVectorString> Map;
	for (PMetaResource *MetaResource : ISMetaData::GetInstanse().GetResources())
	{
		Map.contains(MetaResource->TableName) ? 
			Map[MetaResource->TableName].emplace_back(MetaResource->UID) : 
			Map.insert(MetaResource->TableName, { MetaResource->UID });
	}

	int Removed = 0, Skipped = 0;
	for (const auto &MapItem : Map.toStdMap()) //Обход всех ресурсов
	{
		QString TableName = MapItem.first;
		ISVectorString UIDs = MapItem.second;
		PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);

		//Запрос к текущей таблице ресурсов
		ISQuery qSelect("SELECT " + MetaTable->Alias + "_uid FROM " + TableName);
		qSelect.SetShowLongQuery(false);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				ISUuid ResourceUID = qSelect.ReadColumn(MetaTable->Alias + "_uid");
				if (!VectorContains<QString>(UIDs, ResourceUID)) //Если ресурс из базы не найден в мета-данных
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
							ISLOGGER_ERROR("Error delete resource: " + qDeleteResources.GetErrorString());
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
			ISLOGGER_ERROR(qSelect.GetErrorString());
		}
	}

	ISLOGGER_EMPTY();
	Removed == Skipped ? 
		ISLOGGER_UNKNOWN("Not found obsolete resources") :
		ISLOGGER_UNKNOWN(QString("Removed resources: %1. Skipped resources: %2").arg(Removed).arg(Skipped));
	return true;
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::ShowResourceConsole(PMetaTable *MetaTable, const ISUuid &ResourceUID)
{
	ISLOGGER_EMPTY();
	ISLOGGER_UNKNOWN("Table name: " + MetaTable->Name);
	ISQuery qSelect("SELECT * FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_uid = :ResourceUID");
	qSelect.BindValue(":ResourceUID", ResourceUID);
	if (qSelect.ExecuteFirst())
	{
		QSqlRecord SqlRecord = qSelect.GetRecord();
		for (int i = 0; i < SqlRecord.count(); ++i)
		{
			ISLOGGER_UNKNOWN(QString("%1: %2").arg(SqlRecord.field(i).name()).arg(SqlRecord.value(i).toString()));
		}
	}
}
//-----------------------------------------------------------------------------
