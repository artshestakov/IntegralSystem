#include "CGDatabase.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISLogger.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
//-----------------------------------------------------------------------------
static std::string QS_FOREIGN = PREPARE_QUERY("SELECT COUNT(*) "
    "FROM information_schema.constraint_table_usage "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "AND constraint_name = $1");
//-----------------------------------------------------------------------------
static std::string QC_FOREIGN = "ALTER TABLE public.%1 "
"ADD CONSTRAINT %2 FOREIGN KEY (%3) "
"REFERENCES public.%4(%5_id) "
"ON DELETE NO ACTION "
"ON UPDATE NO ACTION "
"NOT DEFERRABLE";
//-----------------------------------------------------------------------------
static std::string QS_FUNCTION = PREPARE_QUERY("SELECT proname || '(' || pg_get_function_arguments(oid) || ')' AS function_name "
    "FROM pg_proc "
    "WHERE pronamespace = (SELECT oid FROM pg_namespace WHERE nspname = current_schema()) "
    "AND probin IS NULL "
    "ORDER BY function_name");
//-----------------------------------------------------------------------------
static std::string QS_COLUMN = PREPARE_QUERY("SELECT COUNT(*) "
    "FROM information_schema.columns "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "AND lower(table_name) = lower($1) "
    "AND lower(column_name) = lower($2)");
//-----------------------------------------------------------------------------
static std::string QS_INDEXES = PREPARE_QUERY("SELECT COUNT(*) FROM pg_indexes WHERE schemaname = current_schema() AND tablename = :TableName AND indexname = :IndexName");
//-----------------------------------------------------------------------------
static std::string QC_INDEX = "CREATE %1 INDEX %2 ON public.%3 USING btree(%4)";
//-----------------------------------------------------------------------------
static std::string QS_INDEX = PREPARE_QUERY("SELECT indisunique "
    "FROM pg_indexes "
    "JOIN pg_class c ON c.relname = indexname "
    "JOIN pg_index ON indexrelid = c.oid "
    "WHERE schemaname = current_schema() "
    "AND indexname = :IndexName");
//-----------------------------------------------------------------------------
static std::string QD_INDEX = PREPARE_QUERY("DROP INDEX public.%1");
//-----------------------------------------------------------------------------
static std::string Q_REINDEX = "REINDEX INDEX %1";
//-----------------------------------------------------------------------------
static std::string QS_SEQUENCES = PREPARE_QUERY("SELECT COUNT(*) "
    "FROM information_schema.sequences "
    "WHERE sequence_catalog = current_database() "
    "AND sequence_name = $1");
//-----------------------------------------------------------------------------
static std::string QS_TABLE = PREPARE_QUERY("SELECT COUNT(*) "
    "FROM pg_tables "
    "WHERE schemaname = current_schema() "
    "AND lower(tablename) = lower($1)");
//-----------------------------------------------------------------------------
static std::string QS_COLUMNS = PREPARE_QUERY("SELECT column_name AS column_full_name, split_part(column_name, '_', 2) AS column_name, column_default, is_nullable::BOOLEAN, data_type, character_maximum_length "
    "FROM information_schema.columns "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "AND lower(table_name) = lower($1) "
    "ORDER BY ordinal_position");
//-----------------------------------------------------------------------------
static std::string QS_OLD_COLUMNS = PREPARE_QUERY("SELECT "
    "column_name AS column_full_name, "
    "substr(column_name, position('_' IN column_name) + 1, length(column_name) - position('_' IN column_name)) AS column_name "
    "FROM information_schema.columns "
    "WHERE table_catalog = current_database() "
    "AND table_schema = current_schema() "
    "AND lower(table_name) = lower($1) "
    "ORDER BY ordinal_position");
//-----------------------------------------------------------------------------
bool CGDatabase::Foreign_Create(PMetaForeign *MetaForeign, std::string &ErrorString)
{
    PMetaTable *MetaTable = ISMetaData::Instance().GetTable(MetaForeign->TableName);
    PMetaTable *MetaTableForeign = ISMetaData::Instance().GetTable(MetaForeign->ForeignClass);

    std::string SqlText;
    SqlText += "ALTER TABLE public." + MetaTable->Name + " \n";
    SqlText += "ADD CONSTRAINT " + MetaForeign->GetName() + " FOREIGN KEY (" + MetaTable->Alias + '_' + MetaForeign->Field + ") \n";
    SqlText += "REFERENCES public." + MetaTableForeign->Name + '(' + MetaTableForeign->Alias + '_' + MetaForeign->ForeignField + ") \n";
    SqlText += "ON DELETE CASCADE \n";
    SqlText += "ON UPDATE NO ACTION \n";
    SqlText += "NOT DEFERRABLE;";

    ISQuery qCreateForeign(SqlText);
    qCreateForeign.SetShowLongQuery(false);
    bool Result = qCreateForeign.Execute();
    if (!Result)
    {
        ErrorString = qCreateForeign.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Foreign_Update(PMetaForeign *MetaForeign, std::string &ErrorString)
{
    PMetaTable *MetaTable = ISMetaData::Instance().GetTable(MetaForeign->TableName);

    ISQuery qDeleteForeign(ISAlgorithm::StringF("ALTER TABLE public.%s DROP CONSTRAINT %s RESTRICT",
        MetaTable->Name.c_str(), MetaForeign->GetName().c_str()));
    qDeleteForeign.SetShowLongQuery(false);
    bool Result = qDeleteForeign.Execute();
    if (Result)
    {
        Result = Foreign_Create(MetaForeign, ErrorString);
    }
    else
    {
        ErrorString = qDeleteForeign.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Foreign_Exist(PMetaForeign *MetaForeign, bool &Exist, std::string &ErrorString)
{
    ISQuery qSelect(QS_FOREIGN);
    qSelect.SetShowLongQuery(false);
    qSelect.BindString(MetaForeign->GetName());
    bool Result = qSelect.ExecuteFirst();
    if (Result)
    {
        Exist = qSelect.ReadColumn_Int(0) > 0;
    }
    else
    {
        ErrorString = qSelect.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Function_Create(PMetaFunction *MetaFunction, std::string &ErrorString)
{
    ISQuery qCreateFunction(MetaFunction->Text);
    qCreateFunction.SetShowLongQuery(false);
    bool Result = qCreateFunction.Execute();
    if (Result)
    {
        Result = Helper_CommentFunction(MetaFunction, ErrorString);
    }
    else
    {
        ErrorString = qCreateFunction.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Function_Delete(std::string &ErrorString)
{
    ISQuery qSelectFunction(QS_FUNCTION);
    qSelectFunction.SetShowLongQuery(false);
    bool Result = qSelectFunction.Execute(), Founded = false;
    if (Result)
    {
        while (qSelectFunction.Next()) //Обходим функции из БД
        {
            std::string FunctionName = qSelectFunction.ReadColumn_String(0);

            //Ищем такую функцию в мета-данных
            for (PMetaFunction *MetaFunction : ISMetaData::Instance().GetFunctions())
            {
                if (MetaFunction->Name == FunctionName) //Функций найдена - выходим из цикла
                {
                    Founded = true;
                    break;
                }
            }

            if (!Founded) //Если функция не найдена - предлагаем удалить
            {
                if (ISConsole::Question(ISAlgorithm::StringF("Delete old function \"%s\"?", FunctionName.c_str()))) //Согласились на удаление
                {
                    ISQuery qDelete("DROP FUNCTION " + FunctionName);
                    qDelete.SetShowLongQuery(false);
                    Result = qDelete.Execute();
                    if (!Result)
                    {
                        ErrorString = qDelete.GetErrorString();
                        break;
                    }
                }
            }
            Founded = false; //Возвращаем значение флага в исходное положение
        }
    }
    else
    {
        ErrorString = qSelectFunction.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
/*bool CGDatabase::Index_Create(PMetaIndex *Index, QString &ErrorString)
{
    QString Fields, SqlText = QC_INDEX.arg(Index->Unique ? "UNIQUE" : QString()).arg(Index->GetName()).arg(Index->TableName);
    if (!Index->Fields.empty())
    {
        for (const QString &String : Index->Fields)
        {
            Fields += (Index->Alias + '_' + String).toLower() + ", ";
        }
        Fields.chop(2);
        SqlText = SqlText.arg(Fields);
    }
    else
    {
        SqlText = SqlText.arg(Index->Alias + '_' + Index->FieldName.toLower());
    }

    ISQuery qCreateIndex;
    qCreateIndex.SetShowLongQuery(false);
    bool Result = qCreateIndex.Execute(SqlText);
    if (!Result)
    {
        ErrorString = qCreateIndex.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGDatabase::Index_Update(PMetaIndex *Index, QString &ErrorString)
{
    ISQuery qSelect(QS_INDEX);
    qSelect.BindValue(":IndexName", Index->GetName());
    qSelect.SetShowLongQuery(false);
    bool Result = qSelect.ExecuteFirst();
    if (Result)
    {
        //Если уникальность индекса в БД не совпадает с мета-данными - обновляем
        if (qSelect.ReadColumn("indisunique").toBool() != Index->Unique)
        {
            Result = Index_Delete(Index, ErrorString);
            if (Result)
            {
                Result = Index_Create(Index, ErrorString);
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
/*bool CGDatabase::Index_Delete(PMetaIndex *Index, QString &ErrorString)
{
    ISQuery qDelete;
    bool Result = qDelete.Execute(QD_INDEX.arg(Index->GetName()));
    if (!Result)
    {
        ErrorString = qDelete.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGDatabase::Index_Exist(PMetaIndex *Index, bool &Exist, QString &ErrorString)
{
    ISQuery qSelectIndex(QS_INDEXES);
    qSelectIndex.SetShowLongQuery(false);
    qSelectIndex.BindValue(":TableName", Index->TableName.toLower());
    qSelectIndex.BindValue(":IndexName", Index->GetName());
    bool Result = qSelectIndex.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectIndex.ReadColumn("count").toInt() > 0;
    }
    else
    {
        ErrorString = qSelectIndex.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGDatabase::Index_CheckForeign(PMetaIndex *Index)
{
    bool Result = true;
    std::vector<PMetaForeign*> Foreigns = ISMetaData::Instance().GetForeigns();
    for (PMetaForeign *MetaForeign : Foreigns)
    {
        Result = Index->TableName.toLower() == MetaForeign->ForeignClass.toLower();
        if (Result)
        {
            Result = Index->FieldName.toLower() == MetaForeign->ForeignField.toLower();
            if (Result)
            {
                break;
            }
        }
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
/*bool CGDatabase::Index_ReIndex(PMetaIndex *Index, QString &ErrorString)
{
    ISQuery qReindex;
    qReindex.SetShowLongQuery(false);
    bool Result = qReindex.Execute(Q_REINDEX.arg(Index->GetName()));
    if (!Result)
    {
        ErrorString = qReindex.GetErrorString();
    }
    return Result;
}*/
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_Insert(PMetaResource *MetaResource, std::string &ErrorString)
{
    std::string TableAlias = ISMetaData::Instance().GetTable(MetaResource->TableName)->Alias;

    std::string InsertText = "INSERT INTO " + MetaResource->TableName + '(' + TableAlias + "_uid, ";
    std::string ValuesText = "VALUES($1, ";

    size_t CountParameters = MetaResource->Parameters.size();
    for (size_t i = 0; i < CountParameters; ++i)
    {
        std::string FieldName = TableAlias + '_' + ISAlgorithm::ConvertMapToKeys(MetaResource->Parameters)[i];
        InsertText += FieldName + ", ";
        ValuesText += '$' + std::to_string(i + 2) + ", ";
    }

    ISAlgorithm::StringChop(InsertText, 2);
    InsertText += ") \n";

    ISAlgorithm::StringChop(ValuesText, 2);
    ValuesText += ")";

    ISQuery qInsertResource(InsertText + ValuesText);
    qInsertResource.SetShowLongQuery(false);
    qInsertResource.BindUID(MetaResource->UID);

    for (size_t i = 0; i < CountParameters; ++i)
    {
        qInsertResource.Bind(ISAlgorithm::ConvertMapToValues(MetaResource->Parameters)[i]);
    }

    if (!qInsertResource.Execute())
    {
        ErrorString = qInsertResource.GetErrorString();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_Update(PMetaResource *MetaResource, std::string &ErrorString)
{
    std::string TableName = MetaResource->TableName,
        TableAlias = ISMetaData::Instance().GetTable(TableName)->Alias,
        ResourceUID = MetaResource->UID;

    if (!Resource_UpdateField(TableName, TableAlias, TableAlias + "_issystem", "false", ResourceUID, ErrorString))
    {
        IS_ASSERT(false, ISAlgorithm::StringF("Error update resource field. TableName: %s FieldName: %s UID: %s Error: %s",
            TableName.c_str(), (TableAlias + "_issystem").c_str(), ResourceUID.c_str(), ErrorString.c_str()));
    }

    for (const auto &Resource : MetaResource->Parameters) //Обход параметров ресурса
    {
        std::string FieldName = Resource.first,
            FieldNameComplete = TableAlias + '_' + Resource.first,
            FieldValue = Resource.second;

        PMetaField *MetaField = ISMetaData::Instance().GetTable(TableName)->GetField(FieldName);
        if (!MetaField->NotNull) //Если поле НЕ ЯВЛЯЕТСЯ обязательным для заполнения - обнулить его
        {
            IS_ASSERT(Resource_ResetField(TableName, TableAlias, FieldNameComplete, ResourceUID, ErrorString),
                ISAlgorithm::StringF("Not reset resource field. TableName: %s. FieldName: %s. UID: %s. Error: %s",
                TableName.c_str(), FieldNameComplete.c_str(), ResourceUID.c_str(), ErrorString.c_str()));
        }

        IS_ASSERT(Resource_UpdateField(TableName, TableAlias, FieldNameComplete, FieldValue, ResourceUID, ErrorString),
            ISAlgorithm::StringF("Not update resource. TableName: %s. FieldName: %s. UID: %s. Error: %s",
                TableName.c_str(), FieldNameComplete.c_str(), ResourceUID.c_str(), ErrorString.c_str()));
    }

    PMetaTable *MetaTable = ISMetaData::Instance().GetTable(TableName);
    for (PMetaField *MetaField : MetaTable->Fields) //Обход пользовательских полей таблицы и их очистка
    {
        if (MetaField->NotNull || !MetaField->QueryText.empty() || MetaField->IsSystem)
        {
            continue;
        }

        if (!ISAlgorithm::VectorContains(ISAlgorithm::ConvertMapToKeys(MetaResource->Parameters), MetaField->Name))
        {
            IS_ASSERT(Resource_ResetField(TableName, TableAlias, TableAlias + '_' + MetaField->Name, ResourceUID, ErrorString),
                ISAlgorithm::StringF("Not reset resource field. TableName: %s. FieldName: %s. UID: %s. Error: %s",
                    TableName.c_str(), (TableAlias + '_' + MetaField->Name).c_str(), ResourceUID.c_str(), ErrorString.c_str()));
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_Exist(PMetaResource *MetaResource, bool &Exist, std::string &ErrorString)
{
    ISQuery qSelectResource(ISAlgorithm::StringF("SELECT COUNT(*) FROM %s WHERE %s_uid = $1",
        MetaResource->TableName.c_str(), ISMetaData::Instance().GetTable(MetaResource->TableName)->Alias.c_str()));
    qSelectResource.SetShowLongQuery(false);
    qSelectResource.BindUID(MetaResource->UID);
    bool Result = qSelectResource.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectResource.ReadColumn_Int(0) > 0;
    }
    else
    {
        ErrorString = qSelectResource.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_ResetField(const std::string &TableName, const std::string &TableAlias, const std::string &FieldName, const std::string &ResourceUID, std::string &ErrorString)
{
    ISQuery qReset(ISAlgorithm::StringF("UPDATE %s SET %s = NULL WHERE %s_uid = $1",
        TableName.c_str(), FieldName.c_str(), TableAlias.c_str()));
    qReset.SetShowLongQuery(false);
    qReset.BindUID(ResourceUID);
    if (!qReset.Execute())
    {
        ErrorString = qReset.GetErrorString();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Resource_UpdateField(const std::string &TableName, const std::string &TableAlias, const std::string &FieldName, const std::string &Value, const std::string &ResourceUID, std::string &ErrorString)
{
    ISQuery qUpdate(ISAlgorithm::StringF("UPDATE %s SET %s = $1 WHERE %s_uid = $2",
        TableName.c_str(), FieldName.c_str(), TableAlias.c_str()));
    qUpdate.SetShowLongQuery(false);
    qUpdate.Bind(Value);
    qUpdate.BindUID(ResourceUID);
    if (!qUpdate.Execute())
    {
        ErrorString = qUpdate.GetErrorString();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Sequence_Create(const std::string &TableName, std::string &ErrorString)
{
    ISQuery qCreateSequence(ISAlgorithm::StringF("CREATE SEQUENCE public.%s INCREMENT 1 MINVALUE 1 START 1 CACHE 1", Sequence_GetName(TableName).c_str()));
    qCreateSequence.SetShowLongQuery(false);
    bool Result = qCreateSequence.Execute();
    if (!Result)
    {
        ErrorString = qCreateSequence.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Sequence_Exist(const std::string &TableName, bool &Exist, std::string &ErrorString)
{
    ISQuery qSelectSequences(QS_SEQUENCES);
    qSelectSequences.SetShowLongQuery(false);
    qSelectSequences.BindString(Sequence_GetName(TableName));
    bool Result = qSelectSequences.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectSequences.ReadColumn_Int(0) > 0;
    }
    else
    {
        ErrorString = qSelectSequences.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
std::string CGDatabase::Sequence_GetName(const std::string &TableName)
{
    return ISAlgorithm::StringToLowerGet(TableName + "_sequence");
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_Create(PMetaTable *MetaTable, std::string &ErrorString)
{
    std::string TableName = MetaTable->Name,
        TableAlias = MetaTable->Alias;

    bool Exist = true;
    bool Result = Sequence_Exist(TableName, Exist, ErrorString);
    if (Result)
    {
        if (!Exist)
        {
            Result = Sequence_Create(TableName, ErrorString);
        }
    }

    if (!Result)
    {
        return Result;
    }

    //Заголовок запроса
    std::string SqlText = "CREATE TABLE public." + MetaTable->Name + " \n( \n";

    //Формирование запроса на создание таблицы
    for (PMetaField *MetaField : MetaTable->Fields) //Обход полей таблицы
    {
        if (!MetaField->QueryText.empty())
        {
            continue;
        }

        ISNamespace::FieldType FieldType = MetaField->Type; //Тип поля
        bool FieldNotNull = MetaField->NotNull; //Статус обязательного заполнения поля

        SqlText += "\t" + TableAlias + '_' + MetaField->Name + ' ' + ISMetaData::Instance().GetTypeDB(FieldType);

        if (MetaField->Size > 0) //Если указан размер поля
        {
            SqlText += ISAlgorithm::StringF("(%d)", MetaField->Size);
        }

        if (MetaField->Sequence) //Если поле является последовательностью - устанавливаем счётчик значением по умолчанию
        {
            SqlText += ISAlgorithm::StringF(" DEFAULT nextval('%s'::regclass)", Sequence_GetName(TableName).c_str());
        }
        else //Поле не является поледовательностью - устанавливаем указанное значение по умолчанию
        {
            //Если значние по умолчанию не указано - ставим NULL, иначе - то что указали
            SqlText += " DEFAULT " + (MetaField->DefaultValue.empty() ? "NULL" : MetaField->DefaultValue);
        }

        if (FieldNotNull) //Если поле обязательно для заполнения
        {
            SqlText += " NOT NULL";
        }

        if (MetaField->PrimaryKey) //Если поле является первичным ключом
        {
            SqlText += " PRIMARY KEY";
        }
        SqlText += ",\n";
    }

    //Завершение формирования запроса на создание таблицы
    ISAlgorithm::StringChop(SqlText, 2);
    SqlText += "\n);";

    //Исполнение запроса
    ISQuery qCreateTable(SqlText);
    qCreateTable.SetShowLongQuery(false);
    Result = qCreateTable.Execute();
    if (!Result)
    {
        ErrorString = qCreateTable.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_Update(PMetaTable *MetaTable, std::string &ErrorString)
{
    bool Result = Table_AlterFields(MetaTable, ErrorString);
    if (Result)
    {
        Result = Table_CreateFields(MetaTable, ErrorString);
    }

    if (Result)
    {
        Result = Table_DeleteFields(MetaTable, ErrorString);
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_Exist(PMetaTable *MetaTable, bool &Exist, std::string &ErrorString)
{
    ISQuery qSelectTable(QS_TABLE);
    qSelectTable.SetShowLongQuery(false);
    qSelectTable.BindString(MetaTable->Name);
    bool Result = qSelectTable.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectTable.ReadColumn_Int(0) > 0;
    }
    else
    {
        ErrorString = qSelectTable.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_AlterFields(PMetaTable *MetaTable, std::string &ErrorString)
{
    ISQuery qSelectColumns(QS_COLUMNS);
    qSelectColumns.SetShowLongQuery(false);
    qSelectColumns.BindString(MetaTable->Name);
    bool Result = qSelectColumns.Execute();
    if (Result)
    {
        while (qSelectColumns.Next())
        {
            std::string ColumnNameFull = qSelectColumns.ReadColumn_String(0);
            std::string ColumnName = qSelectColumns.ReadColumn_String(1);
            std::string ColumnDefaultValue = qSelectColumns.ReadColumn_String(2);
            bool ColumnNotNull = !qSelectColumns.ReadColumn_Bool(3);
            std::string ColumnType = ISAlgorithm::StringToUpperGet(qSelectColumns.ReadColumn_String(4));
            int ColumnSize = qSelectColumns.ReadColumn_Int(5);

            PMetaField *MetaField = ISMetaData::Instance().GetTable(MetaTable->Name)->GetField(ColumnName);
            if (!MetaField)
            {
                continue;
            }

            std::string MetaType = ISMetaData::Instance().GetTypeDB(MetaField->Type);

            //Проверка соответствия типов
            if (ColumnType != MetaType || ColumnSize != MetaField->Size)
            {
                std::string QueryText = "ALTER TABLE public." + MetaTable->Name + " \n";
                QueryText += "ALTER COLUMN \"" + MetaTable->Alias + '_' + ISAlgorithm::StringToLowerGet(MetaField->Name) + "\" TYPE " + MetaType;
                QueryText += MetaField->Size > 0 ? ISAlgorithm::StringF("(%d) \n", MetaField->Size) : " \n";
                QueryText += "USING \"" + MetaTable->Alias + '_' + ISAlgorithm::StringToLowerGet(MetaField->Name) + "\"::" + MetaType;

                ISQuery qAlterType(QueryText);
                qAlterType.SetShowLongQuery(false);
                Result = qAlterType.Execute();
                if (!Result)
                {
                    ErrorString = qAlterType.GetErrorString();
                    break;
                }
            }

            //Если поле является ID - то не продолжаем дальше
            if (ColumnName == "id")
            {
                continue;
            }

            //Проверка соответствия значения по умолчанию
            if (ColumnDefaultValue != MetaField->DefaultValue)
            {
                //Если в мета-данных значение по умолчанию не указано - удаляем текущее (даже если его нет), иначе - изменяем
                std::string QueryText = "ALTER TABLE public." + MetaTable->Name +
                    " ALTER COLUMN \"" + ColumnNameFull + "\"" + (MetaField->DefaultValue.empty() ? " DROP DEFAULT" : (" SET DEFAULT " + MetaField->DefaultValue));
                ISQuery qAlterColumn(QueryText);
                qAlterColumn.SetShowLongQuery(false);
                Result = qAlterColumn.Execute();
                if (!Result)
                {
                    ErrorString = qAlterColumn.GetErrorString();
                    break;
                }
            }

            //Проверка соответствия флага обязательного заполнения
            if (ColumnNotNull != MetaField->NotNull)
            {
                std::string QueryText;
                if (!ColumnNotNull && MetaField->NotNull)
                {
                    QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN \"" + MetaTable->Alias + '_' + MetaField->Name + "\" SET NOT NULL";
                }
                else if (ColumnNotNull && !MetaField->NotNull)
                {
                    QueryText = "ALTER TABLE public." + MetaTable->Name + " ALTER COLUMN \"" + MetaTable->Alias + '_' + MetaField->Name + "\" DROP NOT NULL";
                }

                ISQuery qAlterNotNull(QueryText);
                qAlterNotNull.SetShowLongQuery(false);
                Result = qAlterNotNull.Execute();
                if (!Result)
                {
                    ErrorString = qAlterNotNull.GetErrorString();
                    break;
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
bool CGDatabase::Table_CreateFields(PMetaTable *MetaTable, std::string &ErrorString)
{
    bool Result = true, Exist = true;
    for (PMetaField *MetaField : MetaTable->Fields) //Обход полей
    {
        if (!MetaField->QueryText.empty())
        {
            continue;
        }

        std::string FieldName = MetaTable->Alias + '_' + MetaField->Name;
        if (Helper_ExistField(MetaTable, FieldName, Exist, ErrorString))
        {
            if (Exist) //Если поле существует - идём к следующему
            {
                continue;
            }
            //Поле не существует - создаём

            std::string AddColumn = "ALTER TABLE public." + MetaTable->Name + " \n" +
                "ADD COLUMN \"" + ISAlgorithm::StringToLowerGet(FieldName) + "\" " + ISMetaData::Instance().GetTypeDB(MetaField->Type);

            if (MetaField->Size) //Если указан размер поля
            {
                AddColumn += ISAlgorithm::StringF("(%d)", MetaField->Size);
            }

            if (!MetaField->DefaultValue.empty()) //Если указано значение по умолчанию
            {
                AddColumn += " DEFAULT " + MetaField->DefaultValue; //Указание NULL без кавычек
            }

            if (MetaField->NotNull)
            {
                AddColumn += " NOT NULL";
            }

            ISQuery qAddColumn(AddColumn);
            qAddColumn.SetShowLongQuery(false);
            Result = qAddColumn.Execute();
            if (Result)
            {
                ISLOGGER_I(__CLASS__, "Add field: %s_%s", MetaTable->Alias.c_str(), MetaField->Name.c_str());
            }
            else
            {
                ErrorString = qAddColumn.GetErrorString();
                break;
            }
        }
        else
        {
            break;
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Table_DeleteFields(PMetaTable *MetaTable, std::string &ErrorString)
{
    ISQuery qSelectColumns(QS_OLD_COLUMNS);
    qSelectColumns.SetShowLongQuery(false);
    qSelectColumns.BindString(MetaTable->Name);
    bool Result = qSelectColumns.Execute();
    if (Result)
    {
        while (qSelectColumns.Next())
        {
            std::string FieldFullName = qSelectColumns.ReadColumn_String(0);
            std::string FieldName = qSelectColumns.ReadColumn_String(1);

            //Если поле существует в базе, но не существует в мета-данных - предлагаем удалить
            //или если это поле есть в базе и оно имеет мета-запрос (т.е. оно виртуальное) - предлагаем удалить
            //Во второму случае такое условие можно объяснить так: было поле в базе, потом его сделали виртуальным - соответственно оно в базе на ни к чему - удаляем
            if (!MetaTable->GetField(FieldName) || (MetaTable->GetField(FieldName) && !MetaTable->GetField(FieldName)->QueryText.empty()))
            {
                if (ISConsole::Question(ISAlgorithm::StringF("The field \"%s\" in the table \"%s\" is out of date, delete?",
                    FieldFullName.c_str(), MetaTable->Name.c_str()))) //Пользователь согласился удалить поле - удаляем
                {
                    ISQuery qDropColumn(ISAlgorithm::StringF("ALTER TABLE public.%s DROP COLUMN \"%s\"",
                        MetaTable->Name.c_str(), FieldFullName.c_str()));
                    qDropColumn.SetShowLongQuery(false);
                    Result = qDropColumn.Execute();
                    if (!Result)
                    {
                        ErrorString = qDropColumn.GetErrorString();
                        break;
                    }
                }
                else //Не согласен удалять поле - пропускаем и идём дальше
                {
                    ISLOGGER_I(__CLASS__, "Skip deletion field: %s", FieldFullName);
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
bool CGDatabase::Helper_ExistField(PMetaTable *MetaTable, const std::string &ColumnName, bool &Exist, std::string &ErrorString)
{
    ISQuery qSelectColumn(QS_COLUMN);
    qSelectColumn.SetShowLongQuery(false);
    qSelectColumn.BindString(MetaTable->Name);
    qSelectColumn.BindString(ColumnName);
    bool Result = qSelectColumn.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectColumn.ReadColumn_Int(0) > 0;
    }
    else
    {
        ErrorString = qSelectColumn.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_CommentTable(PMetaTable *MetaTable, std::string &ErrorString)
{
    rapidjson::Document JsonDocument(rapidjson::Type::kObjectType);
    auto &Allocator = JsonDocument.GetAllocator();

    JsonDocument.AddMember("Name", JSON_STRINGA(MetaTable->Name.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("UID", JSON_STRINGA(MetaTable->UID.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("Alias", JSON_STRINGA(MetaTable->Alias.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("LocalName", JSON_STRINGA(MetaTable->LocalName.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("LocalListName", JSON_STRINGA(MetaTable->LocalListName.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("TitleName", JSON_STRINGA(MetaTable->TitleName.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("ShowOnly", MetaTable->ShowOnly, Allocator);
    JsonDocument.AddMember("IsSystem", MetaTable->IsSystem, Allocator);

    rapidjson::StringBuffer JsonBuffer;
    JsonBuffer.Clear();

    rapidjson::PrettyWriter<rapidjson::StringBuffer> JsonWriter(JsonBuffer);
    JsonDocument.Accept(JsonWriter);
    std::string CommentText = JsonBuffer.GetString();

    ISQuery qComment(ISAlgorithm::StringF("COMMENT ON TABLE public.%s IS '%s'",
        MetaTable->Name.c_str(), CommentText.c_str()));
    qComment.SetShowLongQuery(false);
    bool Result = qComment.Execute();
    if (!Result)
    {
        ErrorString = qComment.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_CommentField(PMetaTable *MetaTable, PMetaField *MetaField, std::string &ErrorString)
{
    if (!MetaField->QueryText.empty()) //Если поле является виртуальным - выходим с положительным результатом
    {
        return true;
    }

    rapidjson::Document JsonDocument(rapidjson::Type::kObjectType);
    auto &Allocator = JsonDocument.GetAllocator();

    JsonDocument.AddMember("UID", JSON_STRINGA(MetaField->UID.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("Name", JSON_STRINGA(MetaField->Name.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("Type", JSON_STRINGA(ISMetaData::Instance().GetTypeDB(MetaField->Type).c_str(), Allocator), Allocator);
    JsonDocument.AddMember("Size", MetaField->Size, Allocator);
    JsonDocument.AddMember("LabelName", JSON_STRINGA(MetaField->LabelName.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("LocalListName", JSON_STRINGA(MetaField->LocalListName.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("NotNull", MetaField->NotNull, Allocator);
    JsonDocument.AddMember("ReadOnly", MetaField->ReadOnly, Allocator);
    JsonDocument.AddMember("HideFromObject", MetaField->HideFromObject, Allocator);
    JsonDocument.AddMember("HideFromList", MetaField->HideFromList, Allocator);
    JsonDocument.AddMember("Hint", JSON_STRINGA(MetaField->Hint.c_str(), Allocator), Allocator);
    JsonDocument.AddMember("IsSystem", MetaField->IsSystem, Allocator);

    rapidjson::StringBuffer JsonBuffer;
    JsonBuffer.Clear();

    rapidjson::PrettyWriter<rapidjson::StringBuffer> JsonWriter(JsonBuffer);
    JsonDocument.Accept(JsonWriter);
    std::string CommentText = JsonBuffer.GetString();

    ISQuery qComment(ISAlgorithm::StringF("COMMENT ON COLUMN public.%s.%s IS '%s'",
        MetaTable->Name.c_str(), (MetaTable->Alias + '_' + MetaField->Name).c_str(), CommentText.c_str()));
    qComment.SetShowLongQuery(false);
    bool Result = qComment.Execute();
    if (!Result)
    {
        ErrorString = qComment.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool CGDatabase::Helper_CommentFunction(PMetaFunction *MetaFunction, std::string &ErrorString)
{
    ISQuery qComment(ISAlgorithm::StringF("COMMENT ON FUNCTION %s IS '%s'",
        MetaFunction->Name.c_str(), MetaFunction->Comment.c_str()));
    qComment.SetShowLongQuery(false);
    bool Result = qComment.Execute();
    if (!Result)
    {
        ErrorString = qComment.GetErrorString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
