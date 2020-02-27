#include "CGConfiguratorService.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISCommandLine.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString Q_VACUUM = "VACUUM";
//-----------------------------------------------------------------------------
static QString Q_VACUUM_ANALYZE = "VACUUM ANALYZE";
//-----------------------------------------------------------------------------
static QString Q_VACUUM_FULL = "VACUUM FULL";
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT indexname "
										  "FROM pg_indexes "
										  "WHERE schemaname = current_schema()");
//-----------------------------------------------------------------------------
CGConfiguratorService::CGConfiguratorService() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorService::~CGConfiguratorService()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorService::reindex()
{
	ISLOGGER_UNKNOWN("Reindex...");

	int CountTables = ISMetaData::GetInstanse().GetTables().count();
	for (int i = 0; i < CountTables; ++i)
	{
		Progress("Reindex", i, CountTables);

		QString TableName = ISMetaData::GetInstanse().GetTables()[i]->Name;

		ISLOGGER_UNKNOWN("Reindex table: " + TableName);

		ISQuery qReindexTable;
		qReindexTable.SetShowLongQuery(false);
		if (qReindexTable.Execute(QString("REINDEX TABLE %1").arg(TableName)))
		{
			ISLOGGER_UNKNOWN(QString("Reindex table %1 done").arg(TableName));
		}
		else
		{
			ISLOGGER_UNKNOWN(QString("Reindex table %1 error").arg(TableName));
		}
	}

	ISLOGGER_UNKNOWN("Reindex done");
	ISLOGGER_EMPTY();
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::vacuum()
{
	ISLOGGER_DEBUG("Vacuum...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qVacuum;
	qVacuum.SetShowLongQuery(false);
	if (qVacuum.Execute(Q_VACUUM))
	{
		ISLOGGER_INFO("Vacuum done");
	}
	else
	{
		ISLOGGER_WARNING("Vacuum error: " + qVacuum.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::vacuumanalyze()
{
	ISLOGGER_DEBUG("Vacuum analyze...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qVacuumAnalyze;
	qVacuumAnalyze.SetShowLongQuery(false);
	if (qVacuumAnalyze.Execute(Q_VACUUM_ANALYZE))
	{
		ISLOGGER_INFO("Vacuum analyze done");
	}
	else
	{
		ISLOGGER_WARNING("Vacuum analyze error: " + qVacuumAnalyze.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::vacuumfull()
{
	ISLOGGER_DEBUG("Vacuum full...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qVacuumFull;
	qVacuumFull.SetShowLongQuery(false);
	if (qVacuumFull.Execute(Q_VACUUM_FULL))
	{
		ISLOGGER_INFO("Vacuum full done");
	}
	else
	{
		ISLOGGER_WARNING("Vacuum full error: " + qVacuumFull.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::cleartable()
{
	ISLOGGER_UNKNOWN("Input table name:");
	QString InputName = ISCommandLine::GetText(); //Запрос на ввод имени таблицы
	if (InputName.isEmpty())
	{
		ISLOGGER_UNKNOWN("Table name is empty");
		return;
	}

	PMetaClassTable *MetaTable = nullptr;
	QList<PMetaClassTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaClassTable *meta_table : Tables) //Поиск таблицы по введенному имени
	{
		if (meta_table->Name.toLower() == InputName)
		{
			MetaTable = meta_table;
		}
	}

	if (!MetaTable) //Если таблица не найдена
	{
		ISLOGGER_UNKNOWN(QString("Table \"%1\" not found").arg(InputName));
		return;
	}

	//Запрос списка идентификаторов таблицы
	ISQuery qSelect("SELECT " + MetaTable->Alias + "_id FROM " + MetaTable->Name + " ORDER BY " + MetaTable->Alias + "_id");
	qSelect.SetShowLongQuery(false);
	if (qSelect.Execute())
	{
		int Removed = 1;
		int RecordCount = qSelect.GetCountResultRows();
		while (qSelect.Next()) //Обход идентификаторов
		{
			int ObjectID = qSelect.ReadColumn(MetaTable->Alias + "_id").toInt();
			
			ISQuery qDelete("DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID");
			qDelete.SetShowLongQuery(false);
			qDelete.BindValue(":ObjectID", ObjectID);
			if (qDelete.Execute())
			{
				ISLOGGER_UNKNOWN(QString("Delete record %1 of %2").arg(Removed).arg(RecordCount));
				++Removed;
			}
		}
	}
}
//-----------------------------------------------------------------------------
