#include "CGConfiguratorService.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
//-----------------------------------------------------------------------------
static std::string Q_VACUUM = "VACUUM";
//-----------------------------------------------------------------------------
static std::string Q_VACUUM_ANALYZE = "VACUUM ANALYZE";
//-----------------------------------------------------------------------------
static std::string Q_VACUUM_FULL = "VACUUM FULL";
//-----------------------------------------------------------------------------
static std::string QS_INDEXES = PREPARE_QUERY("SELECT indexname "
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
/*bool CGConfiguratorService::reindex()
{
    ISDEBUG_L("Reindex...");

    bool Result = true;
    for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i)
    {
        Progress("Reindex", i, CountTables);

        QString TableName = ISMetaData::Instance().GetTables()[i]->Name;
        ISDEBUG_L("Reindex table: " + TableName);

        ISQuery qReindexTable;
        qReindexTable.SetShowLongQuery(false);
        Result = qReindexTable.Execute(QString("REINDEX TABLE %1").arg(TableName));
        if (Result)
        {
            ISDEBUG_L(QString("Reindex table %1 done").arg(TableName));
        }
        else
        {
            ISDEBUG_L(QString("Reindex table %1 error").arg(TableName));
            ErrorString = qReindexTable.GetErrorString();
        }
    }
    ISDEBUG_L("Reindex done\n");
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorService::vacuum()
{
    ISDEBUG_D("Vacuum...");

    ISQuery qVacuum;
    qVacuum.SetShowLongQuery(false);
    bool Result = qVacuum.Execute(Q_VACUUM);
    if (Result)
    {
        ISDEBUG_I("Vacuum done");
    }
    else
    {
        ISDEBUG_W("Vacuum error: " + qVacuum.GetErrorString());
        ErrorString = qVacuum.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorService::vacuumanalyze()
{
    ISDEBUG_D("Vacuum analyze...");

    ISQuery qVacuumAnalyze;
    qVacuumAnalyze.SetShowLongQuery(false);
    bool Result = qVacuumAnalyze.Execute(Q_VACUUM_ANALYZE);
    if (Result)
    {
        ISDEBUG_I("Vacuum analyze done");
    }
    else
    {
        ISDEBUG_W("Vacuum analyze error: " + qVacuumAnalyze.GetErrorString());
        ErrorString = qVacuumAnalyze.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorService::vacuumfull()
{
    ISDEBUG_D("Vacuum full...");

    ISQuery qVacuumFull;
    qVacuumFull.SetShowLongQuery(false);
    bool Result = qVacuumFull.Execute(Q_VACUUM_FULL);
    if (Result)
    {
        ISDEBUG_I("Vacuum full done");
    }
    else
    {
        ISDEBUG_W("Vacuum full error: " + qVacuumFull.GetErrorString());
        ErrorString = qVacuumFull.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorService::cleartable()
{
    QString InputName = ISConsole::GetString("Input table name: "); //Запрос на ввод имени таблицы
    if (InputName.isEmpty())
    {
        ISDEBUG_L("Table name is empty");
        return false;
    }

    PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(InputName);
    if (!MetaTable) //Если таблица не найдена
    {
        ISDEBUG_L(QString("Table \"%1\" not found").arg(InputName));
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
                ISDEBUG_L(QString("Delete record %1 of %2").arg(Removed).arg(RecordCount));
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
}*/
//-----------------------------------------------------------------------------
