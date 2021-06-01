#include "CGConfiguratorDelete.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static std::string QU_ADMIN_PASSWORD = PREPARE_QUERY("UPDATE _users SET "
    "usrs_hash = NULL, "
    "usrs_salt = NULL "
    "WHERE usrs_uid = $1");
//-----------------------------------------------------------------------------
static std::string QS_INDEXES = PREPARE_QUERY("SELECT indexname "
    "FROM pg_indexes "
    "WHERE schemaname = current_schema() "
    "AND right(indexname, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static std::string QS_FOREIGNS = PREPARE_QUERY("SELECT constraint_name "
    "FROM information_schema.constraint_table_usage "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "AND right(constraint_name, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static std::string QS_TABLES = PREPARE_QUERY("SELECT lower(tablename) "
    "FROM pg_tables "
    "WHERE schemaname = current_schema() "
    "ORDER BY tablename");
//-----------------------------------------------------------------------------
static std::string QS_COLUMNS = PREPARE_QUERY("SELECT lower(table_name), lower(column_name) "
    "FROM information_schema.columns "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "ORDER BY table_name, ordinal_position");
//-----------------------------------------------------------------------------
static std::string QS_INDEX = PREPARE_QUERY("SELECT indexname "
    "FROM pg_indexes "
    "JOIN pg_class c ON c.relname = indexname "
    "JOIN pg_index ON indexrelid = c.oid "
    "WHERE schemaname = current_schema() "
    "AND right(indexname, 4) != 'pkey'");
//-----------------------------------------------------------------------------
static std::string QS_FOREIGN = PREPARE_QUERY("SELECT table_name, constraint_name "
    "FROM information_schema.table_constraints "
    "WHERE constraint_type = 'FOREIGN KEY' "
    "AND table_schema = current_schema() "
    "ORDER BY constraint_name");
//-----------------------------------------------------------------------------
CGConfiguratorDelete::CGConfiguratorDelete() : CGConfiguratorBase()
{
    RegisterFunction("adminpassword", static_cast<Function>(&CGConfiguratorDelete::adminpassword));
    RegisterFunction("indexes", static_cast<Function>(&CGConfiguratorDelete::indexes));
    RegisterFunction("foreigns", static_cast<Function>(&CGConfiguratorDelete::foreigns));
    RegisterFunction("oldtables", static_cast<Function>(&CGConfiguratorDelete::oldtables));
    RegisterFunction("oldfields", static_cast<Function>(&CGConfiguratorDelete::oldfields));
    RegisterFunction("oldresources", static_cast<Function>(&CGConfiguratorDelete::oldresources));
    RegisterFunction("oldsequence", static_cast<Function>(&CGConfiguratorDelete::oldsequence));
    RegisterFunction("oldindexes", static_cast<Function>(&CGConfiguratorDelete::oldindexes));
    RegisterFunction("oldforeigns", static_cast<Function>(&CGConfiguratorDelete::oldforeigns));
}
//-----------------------------------------------------------------------------
CGConfiguratorDelete::~CGConfiguratorDelete()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::adminpassword()
{
    if (ISConsole::Question("Are you sure?"))
    {
        ISQuery qUpdatePassword(QU_ADMIN_PASSWORD);
        qUpdatePassword.BindUID(DB_SYSTEM_USER_UID);
        qUpdatePassword.SetShowLongQuery(false);
        bool Result = qUpdatePassword.Execute();
        if (Result) //Запрос прошёл успешно
        {
            Result = qUpdatePassword.GetResultAffected() > 0;
            if (Result) //Пароль был удалён
            {
                ISLOGGER_I(__CLASS__, "Password deleted successfully");
            }
            else //Админ в БД не существует - ошибка
            {
                ErrorString = "The administrator account does not exist";
            }
        }
        else //Ошибка запроса
        {
            ErrorString = qUpdatePassword.GetErrorString();
        }
        return Result;
    }
    ErrorString = "You refused";
    return false;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::indexes()
{
    ISQuery qSelectIndexes(QS_INDEXES);
    qSelectIndexes.SetShowLongQuery(false);
    bool Result = qSelectIndexes.Execute();
    if (Result)
    {
        int Deleted = 0, CountIndexes = qSelectIndexes.GetResultRowCount();
        while (qSelectIndexes.Next())
        {
            ISQuery qDeleteIndex(ISAlgorithm::StringF("DROP INDEX public.%s CASCADE",
                qSelectIndexes.ReadColumn(0)));
            qDeleteIndex.SetShowLongQuery(false);
            Result = qDeleteIndex.Execute();
            if (Result)
            {
                ISLOGGER_I(__CLASS__, "Deleted %d of %d indexes", ++Deleted, CountIndexes);
            }
            else
            {
                ErrorString = qDeleteIndex.GetErrorString();
                break;
            }
        }
    }
    else
    {
        ErrorString = qSelectIndexes.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::foreigns()
{
    ISLOGGER_I(__CLASS__, "Deleting foreigns...");

    ISQuery qSelectForeigns(QS_FOREIGNS);
    qSelectForeigns.SetShowLongQuery(false);
    bool Result = qSelectForeigns.Execute();
    if (Result)
    {
        int Deleted = 0, CountForeigns = qSelectForeigns.GetResultRowCount();
        while (qSelectForeigns.Next())
        {
            std::string ForeignName = qSelectForeigns.ReadColumn_String(0);
            std::string TableName;

            ISVectorString VectorString = ISAlgorithm::StringSplit(ForeignName, '_');
            if (VectorString[0].length())
            {
                TableName = VectorString[0];
            }
            else
            {
                TableName = '_' + VectorString[1];
            }


            ISQuery qDeleteForeign(ISAlgorithm::StringF("ALTER TABLE public.%s DROP CONSTRAINT %s RESTRICT",
                TableName.c_str(), ForeignName.c_str()));
            qDeleteForeign.SetShowLongQuery(false);
            Result = qDeleteForeign.Execute();
            if (Result)
            {
                ISLOGGER_I(__CLASS__, "Deleted %d of %d foreigns", ++Deleted, CountForeigns);
            }
            else
            {
                ErrorString = qDeleteForeign.GetErrorString();
                break;
            }
        }
    }
    else
    {
        ErrorString = qSelectForeigns.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldtables()
{
    ISVectorString VectorString;
    std::vector<PMetaTable*> Tables = ISMetaData::Instance().GetTables();
    for (PMetaTable *MetaTable : Tables)
    {
        VectorString.emplace_back(ISAlgorithm::StringToLowerGet(MetaTable->Name));
    }

    ISQuery qSelectTables(QS_TABLES);
    qSelectTables.SetShowLongQuery(false);
    bool Result = qSelectTables.Execute();
    if (Result)
    {
        while (qSelectTables.Next())
        {
            std::string TableName = qSelectTables.ReadColumn_String(0);
            if (!ISAlgorithm::VectorContains(VectorString, TableName))
            {
                if (ISConsole::Question(ISAlgorithm::StringF("Remove table \"%s\"?", TableName.c_str())))
                {
                    ISLOGGER_I(__CLASS__, "Removing table \"%s\"...", TableName.c_str());
                    ISQuery qDeleteTable(ISAlgorithm::StringF("DROP TABLE public.%s", TableName.c_str()));
                    qDeleteTable.SetShowLongQuery(false);
                    Result = qDeleteTable.Execute();
                    if (Result)
                    {
                        ISLOGGER_I(__CLASS__, "Table removed");
                    }
                    else
                    {
                        ErrorString = qDeleteTable.GetErrorString();
                    }
                }
            }
        }
    }
    else
    {
        ErrorString = qSelectTables.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldfields()
{
    std::map<std::string, ISVectorString> Map;
    std::vector<PMetaTable*> Tables = ISMetaData::Instance().GetTables();
    for (PMetaTable *MetaTable : Tables)
    {
        std::string TableName = ISAlgorithm::StringToLowerGet(MetaTable->Name);
        Map.emplace(TableName, ISVectorString());

        for (PMetaField* MetaField : MetaTable->Fields)
        {
            Map[TableName].emplace_back(ISAlgorithm::StringToLowerGet(MetaTable->Alias + '_' + MetaField->Name));
        }
    }

    ISQuery qSelectColumns(QS_COLUMNS);
    qSelectColumns.SetShowLongQuery(false);
    bool Result = qSelectColumns.Execute();
    if (Result)
    {
        while (qSelectColumns.Next())
        {
            std::string TableName = qSelectColumns.ReadColumn_String(0);
            std::string ColumnName = qSelectColumns.ReadColumn_String(1);
            if (Map.find(TableName) != Map.end())
            {
                if (!ISAlgorithm::VectorContains(Map[TableName], ColumnName))
                {
                    if (ISConsole::Question(ISAlgorithm::StringF("Remove column \"%s\" in table \"%s\"?", ColumnName.c_str(), TableName.c_str())))
                    {
                        ISLOGGER_I(__CLASS__, "Removing column \"%s\"...", ColumnName.c_str());
                        ISQuery qDeleteField(ISAlgorithm::StringF("ALTER TABLE public.%s DROP COLUMN %s", TableName.c_str(), ColumnName.c_str()));
                        qDeleteField.SetShowLongQuery(false);
                        Result = qDeleteField.Execute();
                        if (Result)
                        {
                            ErrorString = qDeleteField.GetErrorString();
                        }
                        else
                        {
                            ISLOGGER_I(__CLASS__, "Field removed");
                        }
                    }
                }
            }
        }
    }
    else
    {
        ErrorString = qSelectColumns.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldresources()
{
    std::map<std::string, ISVectorString> Map;
    for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
    {
        if (Map.find(MetaResource->TableName) != Map.end())
        {
            Map[MetaResource->TableName].emplace_back(MetaResource->UID);
        }
        else
        {
            Map.emplace(MetaResource->TableName, ISVectorString{ MetaResource->UID });
        }
    }

    for (const auto &MapItem : Map)
    {
        std::string TableName = MapItem.first;
        ISVectorString UIDs = MapItem.second;
        PMetaTable *MetaTable = ISMetaData::Instance().GetTable(TableName);

        ISQuery qSelect(ISAlgorithm::StringF("SELECT %s_uid FROM %s",
            MetaTable->Alias.c_str(), TableName.c_str()));
        qSelect.SetShowLongQuery(false);
        if (qSelect.Execute())
        {
            while (qSelect.Next())
            {
                std::string ResourceUID = qSelect.ReadColumn_String(0);
                if (!ISAlgorithm::VectorContains(UIDs, ResourceUID))
                {
                    ShowResourceConsole(MetaTable, ResourceUID);
                    if (ISConsole::Question(ISAlgorithm::StringF("Remove resource \"%s\" in table \"%s\"?", ResourceUID.c_str(), TableName.c_str())))
                    {
                        ISQuery qDeleteResources(ISAlgorithm::StringF("DELETE FROM %s WHERE %s_uid = $1",
                            TableName.c_str(), MetaTable->Alias.c_str()));
                        qDeleteResources.SetShowLongQuery(false);
                        qDeleteResources.BindUID(ResourceUID);
                        if (!qDeleteResources.Execute())
                        {
                            ErrorString = qDeleteResources.GetErrorString();
                        }
                    }
                }
            }
        }
        else
        {
            ErrorString = qSelect.GetErrorString();
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldsequence()
{
    std::string Where;
    for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
    {
        Where += ISAlgorithm::StringF("\'%s_sequence\', ",
            ISAlgorithm::StringToLowerGet(MetaTable->Name).c_str());
    }
    ISAlgorithm::StringChop(Where, 2);

    ISQuery qSelect(ISAlgorithm::StringF("SELECT lower(sequence_name) "
        "FROM information_schema.sequences "
        "WHERE sequence_catalog = current_database() "
        "AND sequence_name NOT IN(%s) "
        "ORDER BY sequence_name",
        Where.c_str()));
    qSelect.SetShowLongQuery(false);
    bool Result = qSelect.Execute();
    if (Result)
    {
        while (qSelect.Next())
        {
            std::string SequenceName = qSelect.ReadColumn_String(0);
            if (ISConsole::Question(ISAlgorithm::StringF("Delete sequence \"%s\"?", SequenceName.c_str())))
            {
                ISQuery qDelete(ISAlgorithm::StringF("DROP SEQUENCE public.%s", SequenceName.c_str()));
                qDelete.SetShowLongQuery(false);
                Result = qDelete.Execute();
                if (!Result)
                {
                    ErrorString = qDelete.GetErrorString();
                }
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
bool CGConfiguratorDelete::oldindexes()
{
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
                if (ISConsole::Question(ISAlgorithm::StringF("Delete index %s?", IndexName.c_str())))
                {
                    ISQuery qDeleteIndex(ISAlgorithm::StringF("DROP INDEX public.%s CASCADE", IndexName.c_str()));
                    Result = qDeleteIndex.Execute();
                    if (!Result)
                    {
                        ErrorString = qDeleteIndex.GetErrorString();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        ErrorString = qSelectIndexes.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorDelete::oldforeigns()
{
    ISQuery qSelectForeigns(QS_FOREIGN);
    qSelectForeigns.SetShowLongQuery(false);
    bool Result = qSelectForeigns.Execute();
    if (Result)
    {
        ISVectorString ForeignNames;
        for (PMetaForeign *MetaForeign : ISMetaData::Instance().GetForeigns())
        {
            ForeignNames.emplace_back(MetaForeign->GetName());
        }

        while (qSelectForeigns.Next())
        {
            std::string TableName = qSelectForeigns.ReadColumn_String(0);
            std::string ForeignName = qSelectForeigns.ReadColumn_String(1);
            if (!ISAlgorithm::VectorContains(ForeignNames, ForeignName))
            {
                if (ISConsole::Question(ISAlgorithm::StringF("Delete foreign %s?", ForeignName.c_str())))
                {
                    ISQuery qDeleteForeign(ISAlgorithm::StringF("ALTER TABLE public.%s DROP CONSTRAINT %s RESTRICT",
                        TableName.c_str(), ForeignName.c_str()));
                    qDeleteForeign.SetShowLongQuery(false);
                    Result = qDeleteForeign.Execute();
                    if (!Result)
                    {
                        ErrorString = qDeleteForeign.GetErrorString();
                        break;
                    }
                }
            }
        }
    }
    else
    {
        ErrorString = qSelectForeigns.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
void CGConfiguratorDelete::ShowResourceConsole(PMetaTable *MetaTable, const std::string &ResourceUID)
{
    ISLOGGER_I(__CLASS__, "\nTable name: %s", MetaTable->Name.c_str());

    ISQuery qSelect(ISAlgorithm::StringF("SELECT * FROM %s WHERE %s_uid = $1",
        MetaTable->Name.c_str(), MetaTable->Alias.c_str()));
    qSelect.BindUID(ResourceUID);
    if (qSelect.ExecuteFirst())
    {
        for (int i = 0, c = qSelect.GetResultColumnCount(); i < c; ++i)
        {
            ISLOGGER_I(__CLASS__, "%s: %s", qSelect.GetResultFieldName(i), qSelect.ReadColumn(i));
        }
    }
}
//-----------------------------------------------------------------------------
