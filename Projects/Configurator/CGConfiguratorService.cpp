#include "CGConfiguratorService.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
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
bool CGConfiguratorService::reindex()
{
	ISLOGGER_UNKNOWN("Reindex...");

	bool Result = true;
	for (int i = 0, CountTables = ISMetaData::GetInstanse().GetTables().size(); i < CountTables; ++i)
	{
		Progress("Reindex", i, CountTables);

		QString TableName = ISMetaData::GetInstanse().GetTables()[i]->Name;
		ISLOGGER_UNKNOWN("Reindex table: " + TableName);

		ISQuery qReindexTable;
		qReindexTable.SetShowLongQuery(false);
		Result = qReindexTable.Execute(QString("REINDEX TABLE %1").arg(TableName));
		if (Result)
		{
			ISLOGGER_UNKNOWN(QString("Reindex table %1 done").arg(TableName));
		}
		else
		{
			ISLOGGER_UNKNOWN(QString("Reindex table %1 error").arg(TableName));
			ErrorString = qReindexTable.GetErrorString();
		}
	}

	ISLOGGER_UNKNOWN("Reindex done");
	ISLOGGER_EMPTY();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuum()
{
	ISLOGGER_DEBUG("Vacuum...");

	ISQuery qVacuum;
	qVacuum.SetShowLongQuery(false);
	bool Result = qVacuum.Execute(Q_VACUUM);
	if (Result)
	{
		ISLOGGER_INFO("Vacuum done");
	}
	else
	{
		ISLOGGER_WARNING("Vacuum error: " + qVacuum.GetErrorString());
		ErrorString = qVacuum.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuumanalyze()
{
	ISLOGGER_DEBUG("Vacuum analyze...");

	ISQuery qVacuumAnalyze;
	qVacuumAnalyze.SetShowLongQuery(false);
	bool Result = qVacuumAnalyze.Execute(Q_VACUUM_ANALYZE);
	if (Result)
	{
		ISLOGGER_INFO("Vacuum analyze done");
	}
	else
	{
		ISLOGGER_WARNING("Vacuum analyze error: " + qVacuumAnalyze.GetErrorString());
		ErrorString = qVacuumAnalyze.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuumfull()
{
	ISLOGGER_DEBUG("Vacuum full...");

	ISQuery qVacuumFull;
	qVacuumFull.SetShowLongQuery(false);
	bool Result = qVacuumFull.Execute(Q_VACUUM_FULL);
	if (Result)
	{
		ISLOGGER_INFO("Vacuum full done");
	}
	else
	{
		ISLOGGER_WARNING("Vacuum full error: " + qVacuumFull.GetErrorString());
		ErrorString = qVacuumFull.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::cleartable()
{
	QString InputName = ISConsole::GetString("Input table name: "); //Запрос на ввод имени таблицы
	if (InputName.isEmpty())
	{
		ISLOGGER_UNKNOWN("Table name is empty");
		return false;
	}

	PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(InputName);
	if (!MetaTable) //Если таблица не найдена
	{
		ISLOGGER_UNKNOWN(QString("Table \"%1\" not found").arg(InputName));
		return false;
	}

	//Запрос списка идентификаторов таблицы
	ISQuery qSelect("SELECT " + MetaTable->Alias + "_id FROM " + MetaTable->Name + " ORDER BY " + MetaTable->Alias + "_id");
	qSelect.SetShowLongQuery(false);
	bool Result = qSelect.Execute();
	if (qSelect.Execute())
	{
		int Removed = 1, RecordCount = qSelect.GetCountResultRows();
		while (qSelect.Next()) //Обход идентификаторов
		{
			ISQuery qDelete("DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID");
			qDelete.SetShowLongQuery(false);
			qDelete.BindValue(":ObjectID", qSelect.ReadColumn(MetaTable->Alias + "_id"));
			Result = qDelete.Execute();
			if (Result)
			{
				ISLOGGER_UNKNOWN(QString("Delete record %1 of %2").arg(Removed).arg(RecordCount));
				++Removed;
			}
			else
			{
				ErrorString = qSelect.GetErrorString();
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
