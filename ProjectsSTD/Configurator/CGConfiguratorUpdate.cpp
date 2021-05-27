#include "CGConfiguratorUpdate.h"
#include "ISMetaData.h"
#include "CGDatabase.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISConsole.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static std::string QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT COUNT(*) "
    "FROM _settingsdatabase "
    "WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static std::string QU_SETTINGS_DATABASE = PREPARE_QUERY("UPDATE _settingsdatabase SET "
    "sgdb_issystem = true "
    "WHERE sgdb_uid = :UID");
//-----------------------------------------------------------------------------
static std::string QI_SETTINGS_DATABASE = PREPARE_QUERY("INSERT INTO _settingsdatabase(sgdb_uid, sgdb_issystem) "
    "VALUES(:UID, true)");
//-----------------------------------------------------------------------------
static std::string QS_PROTOCOL = PREPARE_QUERY("SELECT DISTINCT prtc_tablename "
    "FROM _protocol "
    "WHERE prtc_tablename IS NOT NULL "
    "ORDER BY prtc_tablename");
//-----------------------------------------------------------------------------
static std::string QD_PROTOCOL = PREPARE_QUERY("DELETE FROM _protocol "
    "WHERE prtc_tablename = :TableName");
//-----------------------------------------------------------------------------
CGConfiguratorUpdate::CGConfiguratorUpdate() : CGConfiguratorBase()
{
    RegisterFunction("database", static_cast<Function>(&CGConfiguratorUpdate::database));
    RegisterFunction("functions", static_cast<Function>(&CGConfiguratorUpdate::functions));
    RegisterFunction("tables", static_cast<Function>(&CGConfiguratorUpdate::tables));
    RegisterFunction("foreigns", static_cast<Function>(&CGConfiguratorUpdate::foreigns));
    RegisterFunction("resources", static_cast<Function>(&CGConfiguratorUpdate::resources));
}
//-----------------------------------------------------------------------------
CGConfiguratorUpdate::~CGConfiguratorUpdate()
{

}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::database()
{
    bool Result = functions();
    if (Result)
    {
        Result = tables();
    }

    if (Result)
    {
        //Result = comment();
    }

    if (Result)
    {
        //Result = indexes();
    }

    if (Result)
    {
        Result = foreigns();
    }

    if (Result)
    {
        Result = resources();
    }

    if (Result)
    {
        //Result = databasesettings();
    }

    if (Result)
    {
        //Result = protocol();
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::functions()
{
    bool Result = true;

    //������� ������� �� ����-������
    for (size_t i = 0, CountFunctions = ISMetaData::Instance().GetFunctions().size(); i < CountFunctions; ++i)
    {
        Progress("Function", i, CountFunctions);
        PMetaFunction *MetaFunction = ISMetaData::Instance().GetFunctions()[i];
        Result = CGDatabase::Function_Create(MetaFunction, ErrorString);
        if (!Result)
        {
            break;
        }
    }

    if (Result) //���� ���������� ������� ������ ������� - ������� ����������
    {
        Result = CGDatabase::Function_Delete(ErrorString);
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::tables()
{
    bool Result = true, Exist = true;
    for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i) //����� ������
    {
        PMetaTable *MetaTable = ISMetaData::Instance().GetTables()[i];
        Progress("Table", i, CountTables, "TableName: " + MetaTable->Name);
        Result = CGDatabase::Table_Exist(MetaTable, Exist, ErrorString);
        if (Result)
        {
            Result = Exist ? CGDatabase::Table_Update(MetaTable, ErrorString) : CGDatabase::Table_Create(MetaTable, ErrorString);
        }

        if (!Result)
        {
            break;
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
/*bool CGConfiguratorUpdate::comment()
{
    bool Result = true;
    for (size_t i = 0, CountTables = ISMetaData::Instance().GetTables().size(); i < CountTables; ++i) //����� ������
    {
        PMetaTable *MetaTable = ISMetaData::Instance().GetTables()[i];
        Progress("Comment Table", i, CountTables, "TableName: " + MetaTable->Name);
        Result = CGDatabase::Helper_CommentTable(MetaTable, ErrorString);

        if (Result) //��������������� ������� ������ ������� - ������������ ����
        {
            for (size_t j = 0, CountFields = MetaTable->Fields.size(); j < CountFields; ++j)
            {
                PMetaField *MetaField = MetaTable->Fields[j];
                Progress("Comment Field", j, CountFields, "FieldName: " + MetaField->Name);
                Result = CGDatabase::Helper_CommentField(MetaTable, MetaField, ErrorString);
                if (!Result)
                {
                    break;
                }
            }
        }

        if (!Result)
        {
            break;
        }
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorUpdate::indexes()
{
    bool Result = true, Exist = true;
    std::vector<PMetaIndex*> Indexes = ISMetaData::Instance().GetIndexes();
    for (size_t i = 0, CountIndexes = Indexes.size(); i < CountIndexes; ++i) //����� ��������
    {
        PMetaIndex *MetaIndex = Indexes[i];
        Progress("Index", i, CountIndexes, "Table: " + MetaIndex->TableName + ". IndexName: " + MetaIndex->GetName());
        Result = CGDatabase::Index_Exist(MetaIndex, Exist, ErrorString);
        if (Result) //���� �������� ������������� ������� ������ ������� � ��� � �� ��� - ������
        {
            Result = Exist ? CGDatabase::Index_Update(MetaIndex, ErrorString) : CGDatabase::Index_Create(MetaIndex, ErrorString);
        }

        if (!Result)
        {
            break;
        }
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::foreigns()
{
    bool Result = true, Exist = true;

    std::vector<PMetaForeign*> Foreigns = ISMetaData::Instance().GetForeigns();
    for (size_t i = 0, CountForeigns = Foreigns.size(); i < CountForeigns; ++i)
    {
        PMetaForeign *MetaForeign = Foreigns[i];
        Progress("Foreign", i, CountForeigns, "Table: " + MetaForeign->TableName + ". ForeignName: " + MetaForeign->GetName());
        Result = CGDatabase::Foreign_Exist(MetaForeign, Exist, ErrorString);
        if (Result)
        {
            Result = Exist ? CGDatabase::Foreign_Update(MetaForeign, ErrorString) : CGDatabase::Foreign_Create(MetaForeign, ErrorString);
        }

        if (!Result)
        {
            break;
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorUpdate::resources()
{
    bool Result = true, Exist = true;
    for (size_t i = 0, CountResources = ISMetaData::Instance().GetResources().size(); i < CountResources; ++i)
    {
        PMetaResource *MetaResource = ISMetaData::Instance().GetResources()[i];
        Progress("Resource", i, CountResources, "UID: " + MetaResource->UID);

        if (CGDatabase::Resource_Exist(MetaResource, Exist, ErrorString))
        {
            Result = Exist ? CGDatabase::Resource_Update(MetaResource, ErrorString) : CGDatabase::Resource_Insert(MetaResource, ErrorString);
        }

        if (!Result)
        {
            break;
        }
    }

    if (Result) //���� ���������� �������� ������ ������� - ������� ����������
    {
        ISVectorString Tables; //�������, ������� ������� ������������ � ����-������
        for (PMetaResource *MetaResource : ISMetaData::Instance().GetResources())
        {
            if (!ISAlgorithm::VectorContains(Tables, MetaResource->TableName))
            {
                Tables.emplace_back(MetaResource->TableName);
            }
        }

        for (const std::string &TableName : Tables) //������� �������
        {
            std::string TableAlias = ISMetaData::Instance().GetTable(TableName)->Alias;

            ISQuery qSelectResources(ISAlgorithm::StringF("SELECT %s_uid AS uid FROM %s WHERE %s_issystem", TableAlias.c_str(), TableName.c_str(), TableAlias.c_str()));
            Result = qSelectResources.Execute();
            if (Result)
            {
                while (qSelectResources.Next()) //������� ������� ���������� �������
                {
                    std::string ResourceUID = qSelectResources.ReadColumn_String(0);
                    if (!ISMetaData::Instance().CheckExistResource(ResourceUID)) //���� ������ �� ���������� � ����-������ - ���������� ������� ��� �� ��
                    {
                        if (ISConsole::Question(ISAlgorithm::StringF("Delete old resource %s in table %s?", ResourceUID.c_str(), TableName.c_str()))) //������������ ���������� ������� ������ �� ��
                        {
                            ISQuery qDeleteResource(ISAlgorithm::StringF("DELETE FROM %s WHERE %s_uid = $1", TableName.c_str(), TableAlias.c_str()));
                            qDeleteResource.BindUID(ResourceUID);
                            if (!qDeleteResource.Execute())
                            {
                                ISLOGGER_I(__CLASS__, "Error delete resource %s: %s", ResourceUID.c_str(), qDeleteResource.GetErrorString().c_str());
                            }
                        }
                    }
                }
            }
            else
            {
                ErrorString = "Error select resources: " + qSelectResources.GetErrorString();
                break;
            }
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
/*bool CGConfiguratorUpdate::databasesettings()
{
    ISQuery qSelect(QS_SETTINGS_DATABASE);
    qSelect.SetShowLongQuery(false);
    qSelect.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
    bool Result = qSelect.ExecuteFirst();
    if (Result)
    {
        bool IsExist = qSelect.ReadColumn("count").toInt() > 0;
        ISQuery qUpsert(IsExist ? QU_SETTINGS_DATABASE : QI_SETTINGS_DATABASE);
        qUpsert.BindValue(":UID", CONST_UID_SETTINGS_DATABASE);
        qUpsert.SetShowLongQuery(false);
        Result = qUpsert.Execute();
        if (Result)
        {
            ISDEBUG_L(IsExist ? "Database settings updated" : "Database settings created");
        }
        else
        {
            ErrorString = qUpsert.GetErrorString();
        }
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGConfiguratorUpdate::protocol()
{
    ISQuery qSelect(QS_PROTOCOL);
    if (!qSelect.Execute())
    {
        ISDEBUG_L("Error getting unique tables in protocol: " + qSelect.GetErrorString());
        return false;
    }

    //������� ��� �������
    while (qSelect.Next())
    {
        //�������� ����-�������
        QString TableName = qSelect.ReadColumn("prtc_tablename").toString();
        if (ISMetaData::Instance().GetMetaTable(TableName)) //����-������� ������� - ��������� � ���������
        {
            continue;
        }

        //����-������� �� ������� - ���������� �������
        if (!ISConsole::Question(QString("Delete old record with table \"%1\"?").arg(TableName))) //������������ �� ���������� - ��� ������
        {
            continue;
        }

        //������������ ���������� - �������
        ISQuery qDelete(QD_PROTOCOL);
        qDelete.BindValue(":TableName", TableName);
        if (qDelete.Execute()) //�������� ������ �������
        {
            ISDEBUG_L(QString("Deleted success %1 records").arg(qDelete.GetCountAffected()));
        }
        else //������ �������
        {
            ISDEBUG_L("Error deleting records: " + qDelete.GetErrorString());
        }
    }
    return true;
}*/
//-----------------------------------------------------------------------------
