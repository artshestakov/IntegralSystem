#include "CGConfiguratorService.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
//-----------------------------------------------------------------------------
static std::string QS_INDEXES = PREPARE_QUERY("SELECT indexname "
    "FROM pg_indexes "
    "WHERE schemaname = current_schema()");
//-----------------------------------------------------------------------------
CGConfiguratorService::CGConfiguratorService() : CGConfiguratorBase()
{
    RegisterFunction("reindex", static_cast<Function>(&CGConfiguratorService::reindex));
    RegisterFunction("vacuum", static_cast<Function>(&CGConfiguratorService::vacuum));
    RegisterFunction("vacuumanalyze", static_cast<Function>(&CGConfiguratorService::vacuumanalyze));
    RegisterFunction("vacuumfull", static_cast<Function>(&CGConfiguratorService::vacuumfull));
}
//-----------------------------------------------------------------------------
CGConfiguratorService::~CGConfiguratorService()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::reindex()
{
    ISLOGGER_I(__CLASS__, "Reindex...");
    ISTimePoint TimePoint = ISAlgorithm::GetTick();

    bool Result = true;
    for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i)
    {
        Progress("Reindex", i, CountTables);

        std::string TableName = ISMetaData::Instance().GetTables()[i]->Name;
        ISLOGGER_I(__CLASS__, "Reindex table: %s", TableName.c_str());

        ISQuery qReindexTable(ISAlgorithm::StringF("REINDEX TABLE %s", TableName.c_str()));
        qReindexTable.SetShowLongQuery(false);
        Result = qReindexTable.Execute();
        if (!Result)
        {
            ISLOGGER_I(__CLASS__, "Reindex table %s error", TableName.c_str());
            ErrorString = qReindexTable.GetErrorString();
        }
    }
    ISLOGGER_I(__CLASS__, "Reindex done with %d msec", ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint));
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuum()
{
    ISLOGGER_I(__CLASS__, "Vacuum...");
    ISTimePoint TimePoint = ISAlgorithm::GetTick();

    ISQuery qVacuum("VACUUM");
    qVacuum.SetShowLongQuery(false);
    bool Result = qVacuum.Execute();
    if (Result)
    {
        ISLOGGER_I(__CLASS__, "Vacuum done with %d msec", ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint));
    }
    else
    {
        ISLOGGER_I(__CLASS__, "Vacuum error: %s", qVacuum.GetErrorString().c_str());
        ErrorString = qVacuum.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuumanalyze()
{
    ISLOGGER_I(__CLASS__, "Vacuum analyze...");
    ISTimePoint TimePoint = ISAlgorithm::GetTick();

    ISQuery qVacuumAnalyze("VACUUM ANALYZE");
    qVacuumAnalyze.SetShowLongQuery(false);
    bool Result = qVacuumAnalyze.Execute();
    if (Result)
    {
        ISLOGGER_I(__CLASS__, "Vacuum analyze done with %d msec", ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint));
    }
    else
    {
        ISLOGGER_I(__CLASS__, "Vacuum analyze error: %s", qVacuumAnalyze.GetErrorString().c_str());
        ErrorString = qVacuumAnalyze.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorService::vacuumfull()
{
    ISLOGGER_I(__CLASS__, "Vacuum full...");
    ISTimePoint TimePoint = ISAlgorithm::GetTick();

    ISQuery qVacuumFull("VACUUM FULL");
    qVacuumFull.SetShowLongQuery(false);
    bool Result = qVacuumFull.Execute();
    if (Result)
    {
        ISLOGGER_I(__CLASS__, "Vacuum full done with %d msec", ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint));
    }
    else
    {
        ISLOGGER_I(__CLASS__, "Vacuum full error: %s", qVacuumFull.GetErrorString().c_str());
        ErrorString = qVacuumFull.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
