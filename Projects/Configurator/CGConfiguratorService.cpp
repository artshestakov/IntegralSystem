#include "CGConfiguratorService.h"
#include "ISQuery.h"
#include "ISLogger.h"
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
	ISLOGGER_L("Reindex...");

	bool Result = true;
	for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i)
	{
		Progress("Reindex", i, CountTables);

		QString TableName = ISMetaData::Instance().GetTables()[i]->Name;
		ISLOGGER_L("Reindex table: " + TableName);

		ISQuery qReindexTable;
		qReindexTable.SetShowLongQuery(false);
		Result = qReindexTable.Execute(QString("REINDEX TABLE %1").arg(TableName));
		if (Result)
		{
			ISLOGGER_L(QString("Reindex table %1 done").arg(TableName));
		}
		else
		{
			ISLOGGER_L(QString("Reindex table %1 error").arg(TableName));
			ErrorString = qReindexTable.GetErrorString();
		}
	}

	ISLOGGER_L("Reindex done");
	ISLOGGER_N();
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuum()
{
	ISLOGGER_D("Vacuum...");

	ISQuery qVacuum;
	qVacuum.SetShowLongQuery(false);
	bool Result = qVacuum.Execute(Q_VACUUM);
	if (Result)
	{
		ISLOGGER_I("Vacuum done");
	}
	else
	{
		ISLOGGER_W("Vacuum error: " + qVacuum.GetErrorString());
		ErrorString = qVacuum.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuumanalyze()
{
	ISLOGGER_D("Vacuum analyze...");

	ISQuery qVacuumAnalyze;
	qVacuumAnalyze.SetShowLongQuery(false);
	bool Result = qVacuumAnalyze.Execute(Q_VACUUM_ANALYZE);
	if (Result)
	{
		ISLOGGER_I("Vacuum analyze done");
	}
	else
	{
		ISLOGGER_W("Vacuum analyze error: " + qVacuumAnalyze.GetErrorString());
		ErrorString = qVacuumAnalyze.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuumfull()
{
	ISLOGGER_D("Vacuum full...");

	ISQuery qVacuumFull;
	qVacuumFull.SetShowLongQuery(false);
	bool Result = qVacuumFull.Execute(Q_VACUUM_FULL);
	if (Result)
	{
		ISLOGGER_I("Vacuum full done");
	}
	else
	{
		ISLOGGER_W("Vacuum full error: " + qVacuumFull.GetErrorString());
		ErrorString = qVacuumFull.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::cleartable()
{
	QString InputName = ISConsole::GetString("Input table name: "); //������ �� ���� ����� �������
	if (InputName.isEmpty())
	{
		ISLOGGER_L("Table name is empty");
		return false;
	}

	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(InputName);
	if (!MetaTable) //���� ������� �� �������
	{
		ISLOGGER_L(QString("Table \"%1\" not found").arg(InputName));
		return false;
	}

	//������ ������ ��������������� �������
	ISQuery qSelect("SELECT " + MetaTable->Alias + "_id FROM " + MetaTable->Name + " ORDER BY " + MetaTable->Alias + "_id");
	qSelect.SetShowLongQuery(false);
	bool Result = qSelect.Execute();
	if (qSelect.Execute())
	{
		int Removed = 1, RecordCount = qSelect.GetCountResultRows();
		while (qSelect.Next()) //����� ���������������
		{
			ISQuery qDelete("DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID");
			qDelete.SetShowLongQuery(false);
			qDelete.BindValue(":ObjectID", qSelect.ReadColumn(MetaTable->Alias + "_id"));
			Result = qDelete.Execute();
			if (Result)
			{
				ISLOGGER_L(QString("Delete record %1 of %2").arg(Removed).arg(RecordCount));
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
