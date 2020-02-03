#include "CGConfiguratorService.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISCommandLine.h"
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
	ISDebug::ShowString("Reindex...");

	int CountTables = ISMetaData::GetInstanse().GetTables().count();
	for (int i = 0; i < CountTables; ++i)
	{
		Progress("Reindex", i, CountTables);

		QString TableName = ISMetaData::GetInstanse().GetTables().at(i)->GetName();

		ISDebug::ShowString("Reindex table: " + TableName);

		ISQuery qReindexTable;
		qReindexTable.SetShowLongQuery(false);
		if (qReindexTable.Execute(QString("REINDEX TABLE %1").arg(TableName)))
		{
			ISDebug::ShowString(QString("Reindex table %1 done").arg(TableName));
		}
		else
		{
			ISDebug::ShowString(QString("Reindex table %1 error").arg(TableName));
		}
	}

	ISDebug::ShowString("Reindex done");
	ISDebug::ShowEmptyString(true);
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::vacuum()
{
	ISDebug::ShowDebugString("Vacuum...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qVacuum;
	qVacuum.SetShowLongQuery(false);
	if (qVacuum.Execute(Q_VACUUM))
	{
		ISDebug::ShowInfoString("Vacuum done");
	}
	else
	{
		ISDebug::ShowWarningString("Vacuum error: " + qVacuum.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::vacuumanalyze()
{
	ISDebug::ShowDebugString("Vacuum analyze...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qVacuumAnalyze;
	qVacuumAnalyze.SetShowLongQuery(false);
	if (qVacuumAnalyze.Execute(Q_VACUUM_ANALYZE))
	{
		ISDebug::ShowInfoString("Vacuum analyze done");
	}
	else
	{
		ISDebug::ShowWarningString("Vacuum analyze error: " + qVacuumAnalyze.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::vacuumfull()
{
	ISDebug::ShowDebugString("Vacuum full...");
	ISSystem::SleepMilliseconds(ONE_SECOND_TO_MILLISECOND);

	ISQuery qVacuumFull;
	qVacuumFull.SetShowLongQuery(false);
	if (qVacuumFull.Execute(Q_VACUUM_FULL))
	{
		ISDebug::ShowInfoString("Vacuum full done");
	}
	else
	{
		ISDebug::ShowWarningString("Vacuum full error: " + qVacuumFull.GetErrorText());
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorService::cleartable()
{
	ISDebug::ShowString("Input table name:");
	QString InputName = ISCommandLine::GetText(); //Запрос на ввод имени таблицы
	if (!InputName.length())
	{
		ISDebug::ShowString("Table name is empty");
		return;
	}

	PMetaClassTable *MetaTable = nullptr;
	QList<PMetaClassTable*> Tables = ISMetaData::GetInstanse().GetTables();
	for (PMetaClassTable *meta_table : Tables) //Поиск таблицы по введенному имени
	{
		if (meta_table->GetName().toLower() == InputName)
		{
			MetaTable = meta_table;
		}
	}

	if (!MetaTable) //Если таблица не найдена
	{
		ISDebug::ShowString(QString("Table \"%1\" not found").arg(InputName));
		return;
	}

	//Запрос списка идентификаторов таблицы
	ISQuery qSelect("SELECT " + MetaTable->GetAlias() + "_id FROM " + MetaTable->GetName() + " ORDER BY " + MetaTable->GetAlias() + "_id");
	qSelect.SetShowLongQuery(false);
	if (qSelect.Execute())
	{
		int Removed = 1;
		int RecordCount = qSelect.GetCountResultRows();
		while (qSelect.Next()) //Обход идентификаторов
		{
			int ObjectID = qSelect.ReadColumn(MetaTable->GetAlias() + "_id").toInt();
			
			ISQuery qDelete("DELETE FROM " + MetaTable->GetName() + " WHERE " + MetaTable->GetAlias() + "_id = :ObjectID");
			qDelete.SetShowLongQuery(false);
			qDelete.BindValue(":ObjectID", ObjectID);
			if (qDelete.Execute())
			{
				ISDebug::ShowString(QString("Delete record %1 of %2").arg(Removed).arg(RecordCount));
				Removed++;
			}
		}
	}
}
//-----------------------------------------------------------------------------
