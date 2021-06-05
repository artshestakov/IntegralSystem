#include "CGConfiguratorShow.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static std::string QS_TABLES = "SELECT lower(tablename) "
    "FROM pg_tables "
    "WHERE schemaname = current_schema() "
    "ORDER BY tablename";
//-----------------------------------------------------------------------------
static std::string QS_COLUMNS = "SELECT lower(column_name) "
    "FROM information_schema.columns "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "AND table_name = $1 "
    "ORDER BY column_name";
//-----------------------------------------------------------------------------
static std::string QS_INDEX = "SELECT indexname "
    "FROM pg_indexes "
    "JOIN pg_class c ON c.relname = indexname "
    "JOIN pg_index ON indexrelid = c.oid "
    "WHERE schemaname = current_schema() "
    "AND right(indexname, 4) != 'pkey'";
//-----------------------------------------------------------------------------
static std::string QS_FOREIGN = "SELECT constraint_name "
    "FROM information_schema.table_constraints "
    "WHERE constraint_type = 'FOREIGN KEY' "
    "AND table_schema = current_schema() "
    "ORDER BY constraint_name";
//-----------------------------------------------------------------------------
static std::string QS_INFO = "SELECT "
    "(SELECT pg_size_pretty(pg_database_size(current_database()))) AS \"database_size\", "
    "(SELECT pg_catalog.pg_get_userbyid(datdba) AS \"database_owner\" FROM pg_catalog.pg_database WHERE datname = current_database()), "
    "(SELECT pg_encoding_to_char(encoding) AS \"database_encoding\" FROM pg_database WHERE datname = current_database()), "
    "(SELECT now() - pg_postmaster_start_time() AS \"database_uptime\"), "
    "(SELECT pg_backend_pid() AS pid) AS \"database_backend_pid\", "
    "(SELECT version()) AS \"database_version\", "
    "(SELECT setting AS \"database_cluster_path\" FROM pg_settings WHERE name = 'data_directory'), "
    "(SELECT COUNT(*) AS \"table_count\" FROM information_schema.tables WHERE table_catalog = current_database() AND table_schema = current_schema()), "
    "(SELECT COUNT(*) AS \"field_count\" FROM information_schema.columns WHERE table_catalog = current_database() AND table_schema = current_schema()), "
    "(SELECT COUNT(*) AS \"sequence_count\" FROM information_schema.sequences WHERE sequence_catalog = current_database() AND sequence_schema = current_schema()), "
    "(SELECT COUNT(*) AS \"index_count\" FROM pg_indexes WHERE schemaname = current_schema()), "
    "(SELECT COUNT(*) AS \"foreign_count\" FROM information_schema.constraint_table_usage WHERE constraint_catalog = current_database() AND constraint_schema = current_schema())";
//-----------------------------------------------------------------------------
CGConfiguratorShow::CGConfiguratorShow() : CGConfiguratorBase()
{
    RegisterFunction("oldobjects", static_cast<Function>(&CGConfiguratorShow::oldobjects));
    RegisterFunction("databaseinfo", static_cast<Function>(&CGConfiguratorShow::databaseinfo));
}
//-----------------------------------------------------------------------------
CGConfiguratorShow::~CGConfiguratorShow()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldobjects()
{
    int Tables = 0, Fields = 0, Resources = 0, Sequences = 0, Indexes = 0, Foreigns = 0;
    bool Result = oldtables(Tables);
    if (Result)
    {
        Result = oldfields(Fields);
    }

    if (Result)
    {
        Result = oldresources(Resources);
    }

    if (Result)
    {
        Result = oldsequence(Sequences);
    }

    if (Result)
    {
        Result = oldindexes(Indexes);
    }

    if (Result)
    {
        Result = oldforeigns(Foreigns);
    }

    ISLOGGER_I(__CLASS__, "Tables: %d Fields: %d Resources: %d Sequences: %d Indexes: %d Foreigns: %d",
        Tables, Fields, Resources, Sequences, Indexes, Foreigns);
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::databaseinfo()
{
    ISQuery qSelectInfo(QS_INFO);
    qSelectInfo.SetShowLongQuery(false);
    bool Result = qSelectInfo.ExecuteFirst();
    if (Result)
    {
        ISLOGGER_I(__CLASS__, "Size:\t\t%s", qSelectInfo.ReadColumn(0));
        ISLOGGER_I(__CLASS__, "Owner:\t\t%s", qSelectInfo.ReadColumn(1));
        ISLOGGER_I(__CLASS__, "Encoding:\t%s", qSelectInfo.ReadColumn(2));
        ISLOGGER_I(__CLASS__, "Uptime:\t\t%s", qSelectInfo.ReadColumn(3));
        ISLOGGER_I(__CLASS__, "Backend PID:\t%d", qSelectInfo.ReadColumn_Int(4));
        ISLOGGER_I(__CLASS__, "Version:\t%s", qSelectInfo.ReadColumn(5));
        ISLOGGER_I(__CLASS__, "Cluster path:\t%s", qSelectInfo.ReadColumn(6));
        ISLOGGER_I(__CLASS__, "Count tables:\t%d", qSelectInfo.ReadColumn_Int(7));
        ISLOGGER_I(__CLASS__, "Count fields:\t%d", qSelectInfo.ReadColumn_Int(8));
        ISLOGGER_I(__CLASS__, "Count sequence:\t%d", qSelectInfo.ReadColumn_Int(9));
        ISLOGGER_I(__CLASS__, "Count foreigns:\t%d", qSelectInfo.ReadColumn_Int(10));

        //Готовим запрос для расчёта количества строк
        std::string SqlQueryCount = "WITH r AS(\n";
        for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
        {
            SqlQueryCount += ISAlgorithm::StringF("SELECT COUNT(*) FROM %s\nUNION\n", MetaTable->Name.c_str());
        }
        ISAlgorithm::StringChop(SqlQueryCount, 6);
        SqlQueryCount += ") SELECT sum(count) FROM r";

        //Выполняем запрос
        ISQuery qSelectCount(SqlQueryCount);
        qSelectCount.SetShowLongQuery(false);
        if (qSelectCount.ExecuteFirst())
        {
            ISLOGGER_I(__CLASS__, "Count records:\t%d", qSelectCount.ReadColumn_Int(0));
        }
        else
        {
            ISLOGGER_I(__CLASS__, "Error getting count records: %s", qSelectCount.GetErrorString().c_str());
        }
    }
    else
    {
        ISLOGGER_I(__CLASS__, qSelectInfo.GetErrorString().c_str());
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldtables(int &Count)
{
    ISLOGGER_I(__CLASS__, "Search tables...");
    ISQuery qSelectTables(QS_TABLES);
    qSelectTables.SetShowLongQuery(false);
    bool Result = qSelectTables.Execute();
    if (Result)
    {
        while (qSelectTables.Next()) //Обход таблиц базы
        {
            std::string TableName = qSelectTables.ReadColumn_String(0);
            PMetaTable *MetaTable = FoundTable(TableName);
            if (!MetaTable)
            {
                ISLOGGER_I(__CLASS__, TableName.c_str());
                ++Count;
            }
        }
    }
    else
    {
        ErrorString = qSelectTables.GetErrorString();
    }
    ISLOGGER_I(__CLASS__, "");
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldfields(int &Count)
{
    ISLOGGER_I(__CLASS__, "Search fields...");
    ISQuery qSelectTables(QS_TABLES);
    qSelectTables.SetShowLongQuery(false);
    bool Result = qSelectTables.Execute();
    if (Result)
    {
        while (qSelectTables.Next()) //Обход таблиц базы
        {
            std::string TableName = qSelectTables.ReadColumn_String(0);
            PMetaTable *MetaTable = FoundTable(TableName);
            if (MetaTable)
            {
                ISQuery qSelectColumns(QS_COLUMNS);
                qSelectColumns.SetShowLongQuery(false);
                qSelectColumns.BindString(TableName);
                Result = qSelectColumns.Execute();
                if (Result)
                {
                    while (qSelectColumns.Next()) //Обход полей таблицы
                    {
                        std::string ColumnName = qSelectColumns.ReadColumn_String(0);
                        PMetaField *MetaField = FoundField(MetaTable, ColumnName);
                        if (!MetaField)
                        {
                            ISLOGGER_I(__CLASS__, "%s: %s", TableName.c_str(), ColumnName.c_str());
                            ++Count;
                        }
                    }
                }
                else
                {
                    ErrorString = qSelectColumns.GetErrorString();
                    break;
                }
            }
        }
    }
    else
    {
        ErrorString = qSelectTables.GetErrorString();
    }
    ISLOGGER_I(__CLASS__, "");
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldresources(int &Count)
{
    ISLOGGER_I(__CLASS__, "Search resources...");
    std::map<std::string, ISVectorString> Map, MapOutput;
    for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
    {
        std::string TableName = MetaResource->TableName;
        std::string ResourceUID = MetaResource->UID; //to lower?
        if (!Map.count(TableName))
        {
            Map.emplace(TableName, ISVectorString());
        }
        Map[TableName].emplace_back(ResourceUID);
    }
    for (const auto &MapItem : Map)
    {
        std::string TableName = MapItem.first;
        ISVectorString Vector = MapItem.second;

        std::string NotIN;
        for (const std::string &String : Vector)
        {
            NotIN += '\'' + String + "', ";
        }
        ISAlgorithm::StringChop(NotIN, 2);

        std::string TableAlias = ISMetaData::Instance().GetTable(TableName)->Alias;
        std::string SqlText = ISAlgorithm::StringF("SELECT %s_uid FROM %s WHERE %s_uid NOT IN(%s)",
            TableAlias.c_str(), TableName.c_str(), TableAlias.c_str(), NotIN.c_str());

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
                MapOutput[TableName].emplace_back(qSelect.ReadColumn_String(0));
            }
        }
        else
        {
            ErrorString = qSelect.GetErrorString();
            return false;
        }
    }
    for (const auto &OutputItem : MapOutput)
    {
        for (const std::string &String : OutputItem.second)
        {
            ISLOGGER_I(__CLASS__, "%s: %s", OutputItem.first.c_str(), String.c_str());
            ++Count;
        }
    }
    ISLOGGER_I(__CLASS__, "");
    return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldsequence(int &Count)
{
    ISLOGGER_I(__CLASS__, "Search sequences...");
    std::string Where;
    for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
    {
        Where += '\'' + ISAlgorithm::StringToLowerGet(MetaTable->Name) + "_sequence" + "', ";
    }
    ISAlgorithm::StringChop(Where, 2);
    ISQuery qSelectSequences(ISAlgorithm::StringF(
        "SELECT sequence_name "
        "FROM information_schema.sequences "
        "WHERE sequence_catalog = current_database() "
        "AND sequence_name NOT IN(%s) "
        "ORDER BY sequence_name", Where.c_str()));
    qSelectSequences.SetShowLongQuery(false);
    bool Result = qSelectSequences.Execute();
    if (Result)
    {
        while (qSelectSequences.Next())
        {
            ISLOGGER_I(__CLASS__, qSelectSequences.ReadColumn(0));
            ++Count;
        }
    }
    else
    {
        ErrorString = qSelectSequences.GetErrorString();
    }
    ISLOGGER_I(__CLASS__, "");
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldindexes(int &Count)
{
    ISLOGGER_I(__CLASS__, "Search indexes...");
    ISQuery qSelectIndexes(QS_INDEX);
    qSelectIndexes.SetShowLongQuery(false);
    bool Result = qSelectIndexes.Execute();
    if (Result)
    {
        ISVectorString IndexNames;
        for (PMetaIndex *MetaIndex : ISMetaData::Instance().GetIndexes())
        {
            IndexNames.emplace_back(MetaIndex->GetName());
        }

        while (qSelectIndexes.Next())
        {
            std::string IndexName = qSelectIndexes.ReadColumn_String(0);
            if (!ISAlgorithm::VectorContains(IndexNames, IndexName))
            {
                ISLOGGER_I(__CLASS__, IndexName.c_str());
                ++Count;
            }
        }
    }
    else
    {
        ErrorString = qSelectIndexes.GetErrorString();
    }
    ISLOGGER_I(__CLASS__, "");
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorShow::oldforeigns(int &Count)
{
    ISLOGGER_I(__CLASS__, "Search foreigns...");
    ISQuery qSelectForeigns(QS_FOREIGN);
    qSelectForeigns.SetShowLongQuery(false);
    bool Result = qSelectForeigns.Execute();
    if (Result)
    {
        ISVectorString Foreigns;
        for (PMetaForeign *MetaForeign : ISMetaData::Instance().GetForeigns())
        {
            Foreigns.emplace_back(MetaForeign->GetName());
        }

        while (qSelectForeigns.Next())
        {
            std::string ForeignName = qSelectForeigns.ReadColumn_String(0);
            if (!ISAlgorithm::VectorContains(Foreigns, ForeignName))
            {
                ISLOGGER_I(__CLASS__, ForeignName.c_str());
                ++Count;
            }
        }
    }
    else
    {
        ErrorString = qSelectForeigns.GetErrorString();
    }
    ISLOGGER_I(__CLASS__, "");
    return Result;
}
//-----------------------------------------------------------------------------
PMetaTable* CGConfiguratorShow::FoundTable(const std::string &TableName)
{
    for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
    {
        if (ISAlgorithm::StringToLowerGet(MetaTable->Name) == TableName)
        {
            return MetaTable;
        }
    }
    return nullptr;
}
//-----------------------------------------------------------------------------
PMetaField* CGConfiguratorShow::FoundField(PMetaTable *MetaTable, const std::string &ColumnName)
{
    for (PMetaField *MetaField : MetaTable->Fields)
    {
        if (ISAlgorithm::StringToLowerGet(MetaTable->Alias + '_' + MetaField->Name) == ColumnName)
        {
            return MetaField;
        }
    }
    return nullptr;
}
//-----------------------------------------------------------------------------
