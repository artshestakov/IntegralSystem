#include "CGConfiguratorDelete.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISCommandLine.h"
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT indexname FROM pg_indexes WHERE schemaname = current_schema()");
//-----------------------------------------------------------------------------
static QString QD_INDEX = "DROP INDEX public.%1";
//-----------------------------------------------------------------------------
static QString QS_FOREIGNS = PREPARE_QUERY("SELECT constraint_name FROM information_schema.constraint_table_usage WHERE table_catalog = current_database() AND table_schema = current_schema()");
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
void CGConfiguratorDelete::indexes()
{
	ISDebug::ShowDebugString("Deleting indexes...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qSelectIndexes(QS_INDEXES);
	if (qSelectIndexes.Execute())
	{
		int Deleted = 0;
		int CountIndexes = qSelectIndexes.GetCountResultRows();

		while (qSelectIndexes.Next())
		{
			QString IndexName = qSelectIndexes.ReadColumn("indexname").toString();

			ISQuery qDeleteIndex;
			if (qDeleteIndex.Execute(QD_INDEX.arg(IndexName)))
			{
				Deleted++;
			}
		}

		ISDebug::ShowInfoString("Deleted " + QString::number(Deleted) + " of " + QString::number(CountIndexes) + " indexes");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::foreigns()
{
	ISDebug::ShowDebugString("Deleting foreigns...");
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
				Deleted++;
			}
		}

		ISDebug::ShowInfoString("Deleted " + QString::number(Deleted) + " of " + QString::number(CountForeigns) + " foreigns");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::systems()
{
	ISDebug::ShowDebugString("Deleting systems...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qDelete(QD_SYSTEMS);
	if (qDelete.Execute())
	{
		ISDebug::ShowInfoString("Deleting all systems done");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::subsystems()
{
	ISDebug::ShowDebugString("Deleting subsystems...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qDelete(QD_SUB_SYSTEMS);
	if (qDelete.Execute())
	{
		ISDebug::ShowInfoString("Deleting all subsystems done");
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::tables()
{
	QVectorString VectorString;
	QList<PMetaClassTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaClassTable *MetaTable : Tables)
	{
		VectorString.append(MetaTable->GetName().toLower());
	}

	int Removed = 0;
	int Skipped = 0;

	ISQuery qSelectTables(QS_TABLES);
	if (qSelectTables.Execute())
	{
		while (qSelectTables.Next()) //Обход таблиц базы данных
		{
			QString TableName = qSelectTables.ReadColumn("table_name").toString();
			if (!VectorString.contains(TableName)) //Если таблица из базы данных отсутствует в мета-данных
			{
				if (ISCommandLine::Question(QString("Remove table \"%1\"?").arg(TableName))) //Удаление таблицы
				{
					ISQuery qDeleteTable;
					ISDebug::ShowString(QString("Removing table \"%1\"...").arg(TableName));
					bool Executed = qDeleteTable.Execute("DROP TABLE public." + TableName);
					if (Executed)
					{
						Removed++;
						ISDebug::ShowString("Removed table");
					}
				}
				else //Пропустить удаление таблицы
				{
					Skipped++;
				}
			}
		}
	}

	if (Removed == Skipped) //Таблицы для удаления не найдены
	{
		ISDebug::ShowString("Not found obsolete tables");
	}
	else
	{
		ISDebug::ShowString(QString("Removed tables: %1. Skipped tables: %2").arg(Removed).arg(Skipped));
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::fields()
{
	QMap<QString, QVectorString> Map;
	QList<PMetaClassTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaClassTable *MetaTable : Tables)
	{
		QString TableName = MetaTable->GetName().toLower();
		Map.insert(TableName, QVectorString());

		QVector<PMetaClassField*> Fields = MetaTable->GetAllFields();
		for (PMetaClassField* MetaField : Fields)
		{
			Map[TableName].append(MetaTable->GetAlias().toLower() + '_' + MetaField->GetName().toLower());
		}
	}

	int Removed = 0;
	int Skipped = 0;

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
				if (!Map.value(TableName).contains(ColumnName)) //Если поле из базы данных отсутствует в мета-данных
				{
					if (ISCommandLine::Question(QString("Remove column \"%1\" in table \"%2\"?").arg(ColumnName).arg(TableName))) //Удаление поля
					{
						ISQuery qDeleteField;
						ISDebug::ShowString(QString("Removing column \"%1\"...").arg(ColumnName));
						bool Executed = qDeleteField.Execute("ALTER TABLE public." + TableName + " DROP COLUMN " + ColumnName);
						if (Executed)
						{
							Removed++;
							ISDebug::ShowString("Removed column");
						}
					}
					else //Пропустить удаление поля
					{
						Skipped++;
					}
				}
			}
		}
	}

	if (Removed == Skipped) //Поля для удаления не найдены
	{
		ISDebug::ShowString("Not found obsolete fields");
	}
	else
	{
		ISDebug::ShowString(QString("Removed columns: %1. Skipped columns: %2").arg(Removed).arg(Skipped));
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::resources()
{
	QMap<QString, QVectorString> Map;
	for (PMetaClassResource *MetaResource : ISMetaData::GetInstanse().GetResources())
	{
		if (Map.contains(MetaResource->GetTableName()))
		{
			Map[MetaResource->GetTableName()].append(MetaResource->GetUID());
		}
		else
		{
			QVectorString VectorString;
			VectorString.append(MetaResource->GetUID());
			Map.insert(MetaResource->GetTableName(), VectorString);
		}
	}

	int Removed = 0;
	int Skipped = 0;

	for (const auto &MapItem : Map.toStdMap()) //Обход всех ресурсов
	{
		QString TableName = MapItem.first;
		QVectorString UIDs = MapItem.second;
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);

		//Запрос к текущей таблице ресурсов
		ISQuery qSelect("SELECT " + MetaTable->GetAlias() + "_uid FROM " + TableName);
		qSelect.SetShowLongQuery(false);
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				ISUuid ResourceUID = qSelect.ReadColumn(MetaTable->GetAlias() + "_uid");
				if (!UIDs.contains(ResourceUID)) //Если ресурс из базы не найден в мета-данных
				{
					ShowResourceConsole(MetaTable, ResourceUID);
					if (ISCommandLine::Question(QString("Remove resource \"%1\" in table \"%2\"?").arg(ResourceUID).arg(TableName)))
					{
						ISQuery qDeleteResources("DELETE FROM " + TableName + " WHERE " + MetaTable->GetAlias() + "_uid = :ResourceUID");
						qDeleteResources.SetShowLongQuery(false);
						qDeleteResources.BindValue(":ResourceUID", ResourceUID);
						if (qDeleteResources.Execute())
						{
							Removed++;
						}
					}
					else
					{
						Skipped++;
					}
				}
			}
		}
	}

	if (Removed == Skipped) //Ресурсы для удаления не найдены
	{
		ISDebug::ShowString("Not found obsolete resources");
	}
	else
	{
		ISDebug::ShowString(QString("Removed resources: %1. Skipped resources: %2").arg(Removed).arg(Skipped));
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::ShowResourceConsole(PMetaClassTable *MetaTable, const ISUuid &ResourceUID)
{
	ISDebug::ShowEmptyString();
	ISQuery qSelect("SELECT * FROM " + MetaTable->GetName() + " WHERE " + MetaTable->GetAlias() + "_uid = :ResourceUID");
	qSelect.BindValue(":ResourceUID", ResourceUID);
	if (qSelect.ExecuteFirst())
	{
		QSqlRecord SqlRecord = qSelect.GetRecord();
		for (int i = 0; i < SqlRecord.count(); ++i)
		{
			ISDebug::ShowString(QString("%1: %2").arg(SqlRecord.field(i).name()).arg(SqlRecord.value(i).toString()));
		}
	}
}
//-----------------------------------------------------------------------------
