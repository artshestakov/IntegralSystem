#include "ISQuery.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISQuery::ISQuery(PGconn *sql_connection, const std::string &sql_text)
    : ErrorString(STRING_NO_ERROR),
    ErrorNumber(0),
    ErrorSeverity(nullptr),
    ErrorPrimary(nullptr),
    ErrorDetail(nullptr),
    ErrorHint(nullptr),
    ShowLongQuery(true),
    SqlText(sql_text),
    ParameterCount(0),
    SqlConnection(sql_connection),
    SqlResult(nullptr),
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
    ClearResult();
}
//-----------------------------------------------------------------------------
const std::string& ISQuery::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
int ISQuery::GetErrorNumber() const
{
    return ErrorNumber;
}
//-----------------------------------------------------------------------------
const std::string& ISQuery::GetSqlText() const
{
    return SqlText;
}
//-----------------------------------------------------------------------------
void ISQuery::SetSqlQuery(const std::string &sql_text)
{
    SqlText = sql_text;
}
//-----------------------------------------------------------------------------
int ISQuery::GetResultRowCount() const
{
    return CountRows;
}
//-----------------------------------------------------------------------------
int ISQuery::GetResultColumnCount() const
{
    return CountColumns;
}
//-----------------------------------------------------------------------------
Oid ISQuery::GetResultColumnType(size_t Index) const
{
    return PQftype(SqlResult, (int)Index);
}
//-----------------------------------------------------------------------------
const char* ISQuery::GetResultFieldName(int Index) const
{
    return PQfname(SqlResult, Index);
}
//-----------------------------------------------------------------------------
size_t ISQuery::GetResultAffected() const
{
    return (size_t)std::atoi(PQcmdTuples(SqlResult));
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
bool ISQuery::First()
{
    if (CurrentRow + 1 < CountRows)
    {
        ++CurrentRow;
        return true;
    }
    return false;
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
void ISQuery::BindNull()
{
    ParameterValues.emplace_back(nullptr);
    ParameterTypes.emplace_back(InvalidOid);
    ParameterFormats.emplace_back(0);
    ParameterLengths.emplace_back(0);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindString(const std::string &String)
{
    BindString(String.c_str());
}
//-----------------------------------------------------------------------------
void ISQuery::BindString(const char *String)
{
    size_t StringSize = strlen(String);
    if (StringSize == 0)
    {
        ParameterValues.emplace_back(nullptr);
    }
    else
    {
        char *Value = (char *)malloc(StringSize + 1);
        strcpy(Value, String);
        ParameterValues.emplace_back(Value);
    }
    ParameterTypes.emplace_back(VARCHAROID);
    ParameterFormats.emplace_back(0);
    ParameterLengths.emplace_back(0);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindUID(const std::string &UID)
{
    BindString(UID);
    ParameterTypes[ParameterCount - 1] = UUIDOID;
}
//-----------------------------------------------------------------------------
void ISQuery::BindUID(const char *UID)
{
    BindString(UID);
    ParameterTypes[ParameterCount - 1] = UUIDOID;
}
//-----------------------------------------------------------------------------
void ISQuery::BindBool(bool Value)
{
    char *Temp = (char *)malloc(2);
    Temp[0] = Value ? 't' : 'f';
    Temp[1] = CHAR_NULL_TERM;

    ParameterValues.emplace_back(Temp);
    ParameterTypes.emplace_back(BOOLOID);
    ParameterFormats.emplace_back(0);
    ParameterLengths.emplace_back(0);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindBinary(unsigned char *Data, size_t Size)
{
    char *Temp = (char *)malloc(Size + 1);
    memcpy(Temp, Data, Size);

    ParameterValues.emplace_back(Temp);
    ParameterTypes.emplace_back(BYTEAOID);
    ParameterFormats.emplace_back(1);
    ParameterLengths.emplace_back((int)Size);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindUInt(unsigned int Value)
{
    BindUInt64(Value);
    ParameterTypes[ParameterCount - 1] = INT4OID;
}
//-----------------------------------------------------------------------------
void ISQuery::BindUInt64(uint64_t Value)
{
    BindInt64((int64_t)Value);
}
//-----------------------------------------------------------------------------
void ISQuery::BindInt(int Value)
{
    BindInt64(Value);
    ParameterTypes[ParameterCount - 1] = INT4OID;
}
//-----------------------------------------------------------------------------
void ISQuery::BindInt64(int64_t Value)
{
    std::string String = std::to_string(Value);
    char *Temp = (char *)malloc(String.size() + 1);
    strcpy(Temp, String.c_str());

    ParameterValues.emplace_back(Temp);
    ParameterTypes.emplace_back(INT8OID);
    ParameterFormats.emplace_back(0);
    ParameterLengths.emplace_back(0);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindDouble(double Value)
{
    std::string String = std::to_string(Value);
    char *Temp = (char *)malloc(String.size() + 1);
    strcpy(Temp, String.c_str());

    ParameterValues.emplace_back(Temp);
    ParameterTypes.emplace_back(NUMERICOID);
    ParameterFormats.emplace_back(0);
    ParameterLengths.emplace_back(0);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQuery::BindDate(const ISDate &Date)
{
    std::string String = Date.ToString();
    char *Temp = (char *)malloc(String.size() + 1);
    strcpy(Temp, String.c_str());

    ParameterValues.emplace_back(Temp);
    ParameterTypes.emplace_back(DATEOID);
    ParameterFormats.emplace_back(0);
    ParameterLengths.emplace_back(0);
    ++ParameterCount;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute()
{
    //Сделаем очистку (предыдущего результата) на всякий случай
    ClearResult();

    //Проверим, не трубуется ли подготовить запрос
    int ParamCount = 0;
    bool IsNeed = false;
    ISQueryText::Instance().IsNeedPrepare(SqlText, StmtName, ParamCount, IsNeed);
    if (IsNeed) //Запрос нуждается в подготовке
    {
        if (!Prepare(ParamCount)) //Ошибка подготовки
        {
            return false;
        }
        //Запрос подготовлен, запомним это
        ISQueryText::Instance().AddPrepared(SqlText, StmtName, ParamCount);
    }

    //Засекаем время и выполняем запрос
    ISTimePoint TimePoint = ISAlgorithm::GetTick();
    if (ParameterCount > 0) //Есть параметры запроса - параметизированный запрос
    {
        if (StmtName.empty()) //Запрос не был подготовлен - выполняем либо с параметрами, либо без
        {
            SqlResult = ParameterCount > 0 ? PQexecParams(SqlConnection, SqlText.c_str(), (int)ParameterCount, ParameterTypes.data(), ParameterValues.data(), ParameterLengths.data(), ParameterFormats.data(), 0) :
                PQexec(SqlConnection, SqlText.c_str());
        }
        else //Запрос был подготовлен - выполняем подготовленный оператор
        {
            SqlResult = PQexecPrepared(SqlConnection, StmtName.c_str(), (int)ParameterCount, ParameterValues.data(), ParameterLengths.data(), ParameterFormats.data(), 0);
        }

        //Очищаем выделенную память
        for (size_t i = 0; i < ParameterCount; ++i)
        {
            if (ParameterValues[i]) //Если текущий параметр не nullptr - очищаем
            {
                free(ParameterValues[i]);
            }
        }
        ParameterValues.clear();
        ParameterTypes.clear();
        ParameterFormats.clear();
        ParameterLengths.clear();
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
        ErrorString = "Out of memory for PGresult";
        return false;
    }

    //Если запрос выполнялся дольше DB_MAX_QUERY_TIME - выводим предупреждение
    if (ShowLongQuery && Msec > DB_MAX_QUERY_TIME)
    {
        ISLOGGER_W(__CLASS__, "Long query msec: %lld. %s", Msec, SqlText.c_str());
    }

    switch (PQresultStatus(SqlResult)) //Проверяем результат
    {
    case PGRES_TUPLES_OK:
        CountRows = PQntuples(SqlResult);
        CountColumns = PQnfields(SqlResult);
        CurrentRow = -1;
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

    //Получаем информацию об ошибке
    ErrorString = PQerrorMessage(SqlConnection);
    ErrorNumber = std::atoi(PQresultErrorField(SqlResult, PG_DIAG_SQLSTATE));
    ErrorSeverity = PQresultErrorField(SqlResult, PG_DIAG_SEVERITY);
    ErrorPrimary = PQresultErrorField(SqlResult, PG_DIAG_MESSAGE_PRIMARY);
    ErrorDetail = PQresultErrorField(SqlResult, PG_DIAG_MESSAGE_DETAIL);
    ErrorHint = PQresultErrorField(SqlResult, PG_DIAG_MESSAGE_HINT);
    return false;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst()
{
    bool Result = Execute();
    if (Result)
    {
        Result = First();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::IsNull(size_t Index) const
{
    return PQgetisnull(SqlResult, CurrentRow, (int)Index) == 1;
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
double ISQuery::ReadColumn_Double(size_t Index) const
{
    return std::atof(ReadColumn(Index));
}
//-----------------------------------------------------------------------------
unsigned int ISQuery::ReadColumn_UInt(size_t Index) const
{
    return ReadColumn_Int(Index);
}
//-----------------------------------------------------------------------------
uint64_t ISQuery::ReadColumn_Int64(size_t Index) const
{
    return std::atoll(ReadColumn(Index));
}
//-----------------------------------------------------------------------------
uint64_t ISQuery::ReadColumn_UInt64(size_t Index) const
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
    if (IsNull(Index))
    {
        return ISDate();
    }
    //Поле не пустое - парсим

    char *Value = ReadColumn(Index);
    char Year[5] = { Value[0], Value[1], Value[2], Value[3], CHAR_NULL_TERM },
        Month[3] = { Value[5], Value[6], CHAR_NULL_TERM },
        Day[3] = { Value[8], Value[9], CHAR_NULL_TERM };
    return ISDate((unsigned short)std::atoi(Day), (unsigned short)std::atoi(Month), (unsigned short)std::atoi(Year));
}
//-----------------------------------------------------------------------------
ISTime ISQuery::ReadColumn_Time(size_t Index) const
{
    if (IsNull(Index))
    {
        return ISTime();
    }

    char *Value = ReadColumn(Index);
    char Hour[3] = { Value[0], Value[1] },
        Minute[3] = { Value[3], Value[4] },
        Second[3] = { Value[6], Value[7] };
    return ISTime((unsigned short)std::atoi(Hour), (unsigned short)std::atoi(Minute), (unsigned short)std::atoi(Second));
}
//-----------------------------------------------------------------------------
ISDateTime ISQuery::ReadColumn_DateTime(size_t Index) const
{
    if (IsNull(Index))
    {
        return ISDateTime();
    }

    char *Value = ReadColumn(Index);
    char Year[5] = { Value[0], Value[1], Value[2], Value[3], CHAR_NULL_TERM },
        Month[3] = { Value[5], Value[6], CHAR_NULL_TERM },
        Day[3] = { Value[8], Value[9], CHAR_NULL_TERM },
        Hour[3] = { Value[11], Value[12] },
        Minute[3] = { Value[14], Value[15] },
        Second[3] = { Value[17], Value[18] };
    return ISDateTime((unsigned short)std::atoi(Day), (unsigned short)std::atoi(Month), (unsigned short)std::atoi(Year),
        (unsigned short)std::atoi(Hour), (unsigned short)std::atoi(Minute), (unsigned short)std::atoi(Second));
}
//-----------------------------------------------------------------------------
unsigned char* ISQuery::ReadColumn_Binary(size_t Index) const
{
    size_t DataSize = 0;
    return ReadColumn_Binary(Index, DataSize);
}
//-----------------------------------------------------------------------------
unsigned char* ISQuery::ReadColumn_Binary(size_t Index, size_t &DataSize) const
{
    return PQunescapeBytea((const unsigned char *)ReadColumn(Index), &DataSize);
}
//-----------------------------------------------------------------------------
bool ISQuery::Prepare(int ParamCount)
{
    StmtName = std::to_string(CURRENT_THREAD_ID()) + '_' + ISAlgorithm::StringToMD5(SqlText);
    PGresult *STMT = PQprepare(SqlConnection, StmtName.c_str(), SqlText.c_str(), (int)ParamCount, ParameterTypes.data());
    bool Prepared = STMT ? true : false;
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
void ISQuery::ClearResult()
{
    if (SqlResult)
    {
        PQclear(SqlResult);
    }
}
//-----------------------------------------------------------------------------
