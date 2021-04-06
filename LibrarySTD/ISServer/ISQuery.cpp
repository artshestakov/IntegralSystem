#include "ISQuery.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISQuery::ISQuery(PGconn *sql_connection, const std::string &sql_text)
    : ErrorString(STRING_NO_ERROR),
    ShowLongQuery(true),
    SqlText(sql_text),
    ParameterCount(0),
    Prepared(false),
    SqlConnection(sql_connection),
    SqlResult(NULL),
    CountRows(-1),
    CountColumns(-1),
    CurrentRow(-1),
    IsSelect(false)
{

}
//-----------------------------------------------------------------------------
ISQuery::ISQuery(const std::string &sql_text)
    : ISQuery(ISDatabase::Instance().GetDB(DB_CONNECTION_DEFAULT), sql_text)
{

}
//-----------------------------------------------------------------------------
ISQuery::~ISQuery()
{
    if (SqlResult)
    {
        PQclear(SqlResult);
    }
}
//-----------------------------------------------------------------------------
const std::string& ISQuery::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
const std::string& ISQuery::GetSqlText() const
{
    return SqlText;
}
//-----------------------------------------------------------------------------
int ISQuery::GetResultSize() const
{
    return CountRows;
}
//-----------------------------------------------------------------------------
bool ISQuery::GetIsSelect() const
{
    return IsSelect;
}
//-----------------------------------------------------------------------------
void ISQuery::SetShowLongQuery(bool show_long_query)
{
    ShowLongQuery = show_long_query;
}
//-----------------------------------------------------------------------------
void ISQuery::First()
{
    if (CurrentRow + 1 < CountRows)
    {
        ++CurrentRow;
    }
}
//-----------------------------------------------------------------------------
bool ISQuery::Next()
{
    ++CurrentRow;
    if (CurrentRow == CountRows)
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
void ISQuery::BindValue(std::nullptr_t Pointer)
{
    IS_UNUSED(Pointer);
    ParameterValues.emplace_back(std::string());
    ParameterTypes.emplace_back(InvalidOid);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindValue(int Value, Oid OID)
{
    ParameterValues.emplace_back(std::to_string(Value));
    ParameterTypes.emplace_back(OID == InvalidOid ? INT2OID : OID);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindValue(unsigned int Value, Oid OID)
{
    ParameterValues.emplace_back(std::to_string(Value));
    ParameterTypes.emplace_back(OID == InvalidOid ? INT4OID : OID);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindValue(const std::string &Value, Oid OID)
{
    ParameterValues.emplace_back(Value);
    ParameterTypes.emplace_back(OID == InvalidOid ? VARCHAROID : OID);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(bool PrepareQuery, size_t ParamCount)
{
    if (PrepareQuery && !Prepared) //Если запрос нужно подготовить и он ещё не готов - готовим
    {
        if (!Prepare(ParamCount))
        {
            return false;
        }
    }

    //Засекаем время и выполняем запрос
    ISTimePoint TimePoint = ISAlgorithm::GetTick();
    if (ParameterCount > 0) //Есть параметры запроса - параметизированный запрос
    {
        //Получаем размер параметров и пытаемся выделить память для них
        char **ParamValues = (char **)malloc(ParameterCount * (sizeof(char *)));
        if (!ParamValues) //Не удалось выделить память
        {
            ErrorString = "Malloc out of memory";
            return false;
        }

        //Заполяняем параметры
        for (size_t i = 0; i < ParameterCount; ++i)
        {
            //Получаем значение параметра
            std::string String = ParameterValues[i];
            if (String.empty()) //Если значение пустое - интерпретируем его как NULL
            {
                ParamValues[i] = nullptr;
            }
            else //Значение не пустое - анализируем его
            {
                //Выделяем память для параметра
                ParamValues[i] = (char *)malloc(String.size() + 1);
                if (!ParamValues[i]) //Не удалось выделить память
                {
                    free(ParamValues); //Очищаем память под массив
                    ErrorString = "Malloc out of memory";
                    return false;
                }
                strcpy(ParamValues[i], String.c_str()); //Копируем значение в память
            }
        }

        if (StmtName.empty()) //Запрос не был подготовлен - выполняем либо с параметрами, либо без
        {
            SqlResult = ParameterCount > 0 ? PQexecParams(SqlConnection, SqlText.c_str(), (int)ParameterCount, ParameterTypes.data(), ParamValues, NULL, NULL, 0) :
                PQexec(SqlConnection, SqlText.c_str());
        }
        else //Запрос был подготовлен - выполняем подготовленный оператор
        {
            SqlResult = PQexecPrepared(SqlConnection, StmtName.c_str(), (int)ParameterCount, ParamValues, NULL, NULL, 0);
        }

        //Очищаем выделенную память
        for (size_t i = 0; i < ParameterCount; ++i)
        {
            free(ParamValues[i]);
        }
        free(ParamValues);
        ParameterValues.clear();
        ParameterTypes.clear();
        ParameterCount = 0;
    }
    else //Параметров запроса нет - исполняем как есть
    {
        //Если запрос не был подготовлен - исполняем как есть, иначе - выполняем подготовленный оператор
        SqlResult = StmtName.empty() ? PQexec(SqlConnection, SqlText.c_str()) :
            PQexecPrepared(SqlConnection, StmtName.c_str(), 0, NULL, NULL, NULL, 0);
    }

    //Считаем затраченное время на выполнение запроса
    long long Msec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);

    if (!SqlResult) //Не удалось отправить запрос
    {
        ErrorString = "Out of memory for exec";
        return false;
    }

    //Если запрос выполнялся дольше DB_MAX_QUERY_TIME - выводим предупреждение
    if (ShowLongQuery && Msec > DB_MAX_QUERY_TIME)
    {
        ISLOGGER_W(__CLASS__, "Long query msec: %d. %s", Msec, SqlText.c_str());
    }

    switch (PQresultStatus(SqlResult)) //Проверяем результат
    {
    case PGRES_TUPLES_OK:
        CountRows = PQntuples(SqlResult);
        CountColumns = PQnfields(SqlResult);
        IsSelect = true;
        return true;
        break;

    case PGRES_SINGLE_TUPLE:
        IsSelect = true;
        return true;
        break;

    case PGRES_COMMAND_OK:
        return true;
        break;

    default:
        break;
    }

    //Получаем текст ошибки и выходим
    ErrorString = PQerrorMessage(SqlConnection);
    return false;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst()
{
    bool Result = Execute();
    if (Result)
    {
        First();
    }
    return Result;
}
//-----------------------------------------------------------------------------
char* ISQuery::ReadColumn(size_t Index) const
{
    return PQgetvalue(SqlResult, CurrentRow, (int)Index);
}
//-----------------------------------------------------------------------------
std::string ISQuery::ReadColumn_String(size_t Index) const
{
    return ReadColumn(Index);
}
//-----------------------------------------------------------------------------
int ISQuery::ReadColumn_Int(size_t Index) const
{
    return std::atoi(ReadColumn(Index));
}
//-----------------------------------------------------------------------------
unsigned int ISQuery::ReadColumn_UInt(size_t Index) const
{
    return ReadColumn_Int(Index);
}
//-----------------------------------------------------------------------------
ISInt64 ISQuery::ReadColumn_Int64(size_t Index) const
{
    return std::atoll(ReadColumn(Index));
}
//-----------------------------------------------------------------------------
ISUInt64 ISQuery::ReadColumn_UInt64(size_t Index) const
{
    return ReadColumn_Int64(Index);
}
//-----------------------------------------------------------------------------
bool ISQuery::ReadColumn_Bool(size_t Index) const
{
    return (strcmp(ReadColumn(Index), "t") == 0) ? true : false;
}
//-----------------------------------------------------------------------------
ISDate ISQuery::ReadColumn_Date(size_t Index) const
{
    //Если поле содержит пустое значение - возвращаем пустую структуру
    if (PQgetisnull(SqlResult, CurrentRow, (int)Index) == 1)
    {
        return ISDate();
    }
    //Поле не пустое - парсим

    char *Value = ReadColumn(Index);
    char Year[5] = { Value[0], Value[1], Value[2], Value[3], '\0' },
        Month[3] = { Value[5], Value[6], '\0' },
        Day[3] = { Value[8], Value[9], '\0' };
    return{ (unsigned short)atoi(Day), (unsigned short)atoi(Month), (unsigned short)atoi(Year) };
}
//-----------------------------------------------------------------------------
bool ISQuery::Prepare(size_t ParamCount)
{
    //???
    //StmtName = ISAlgorithm::StringToMD5(SqlText);
    PGresult *STMT = PQprepare(SqlConnection, StmtName.c_str(), SqlText.c_str(), (int)ParamCount, ParameterTypes.data());
    Prepared = STMT ? true : false;
    if (Prepared)
    {
        //Проверяем результат, очищаем память и проверяем результат
        ExecStatusType ResultStatus = PQresultStatus(STMT);
        PQclear(STMT);
        Prepared = ResultStatus == PGRES_TUPLES_OK || ResultStatus == PGRES_SINGLE_TUPLE || ResultStatus == PGRES_COMMAND_OK;
        if (!Prepared) //Подготовка не удалась
        {
            ErrorString = PQerrorMessage(SqlConnection);
        }
    }
    else
    {
        ErrorString = "Out of memory for prepare";
    }
    return Prepared;
}
//-----------------------------------------------------------------------------
