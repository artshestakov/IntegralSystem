#include "ISQueryLibPQ.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(const std::string &sql_text, bool prepare)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	SqlConnection(ISDatabase::Instance().GetDBLibPQ(CONNECTION_DEFAULT)),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1),
	IsSelect(false)
{
	if (prepare && !sql_text.empty())
	{
		Prepare();
	}
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text, bool prepare)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	SqlConnection(sql_connection),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1),
	IsSelect(false)
{
	if (prepare && !sql_text.empty())
	{
		Prepare(sql_connection);
	}
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::~ISQueryLibPQ()
{

}
//-----------------------------------------------------------------------------
std::string ISQueryLibPQ::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
int ISQueryLibPQ::GetResultSize() const
{
	return CountRows;
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::GetIsSelect() const
{
	return IsSelect;
}
//-----------------------------------------------------------------------------
void ISQueryLibPQ::SetShowLongQuery(bool show_long_query)
{
	ShowLongQuery = show_long_query;
}
//-----------------------------------------------------------------------------
void ISQueryLibPQ::First()
{
	if (CurrentRow + 1 < CountRows)
	{
		++CurrentRow;
	}
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Next()
{
	++CurrentRow;
	if (CurrentRow == CountRows)
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Execute()
{
	//Засекаем время и выполняем запрос
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	SqlResult = PQexec(SqlConnection, SqlText.c_str());
	long long Msec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
	if (!SqlResult) //Не удалось отправить запрос
	{
		ErrorString = "Out of memory";
		return false;
	}

	//Если запрос выполнялся дольше MAX_QUERY_TIME - выводим предупреждение
	if (ShowLongQuery && Msec > MAX_QUERY_TIME)
	{
		ISLOGGER_W(__CLASS__, QString("Long query msec: %1. %2").arg(Msec).arg(SqlText.c_str()));
	}

	switch (PQresultStatus(SqlResult)) //Проверяем результат
	{
	case PGRES_TUPLES_OK:
		CountRows = PQntuples(SqlResult);
		CountColumns = PQnfields(SqlResult);
		IsSelect = true;
		FillColumnMap();
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
bool ISQueryLibPQ::ExecuteFirst()
{
	bool Result = Execute();
	if (Result)
	{
		First();
	}
	return Result;
}
//-----------------------------------------------------------------------------
Oid ISQueryLibPQ::ColumnType(int Index)
{
	return ColumnMap[Index].Type;
}
//-----------------------------------------------------------------------------
Oid ISQueryLibPQ::ColumnType(const std::string &FieldName)
{
	Oid FieldType = 0;
	for (const auto &MapItem : ColumnMap)
	{
		if (MapItem.second.Name == FieldName)
		{
			FieldType = MapItem.second.Type;
			break;
		}
	}
	return FieldType;
}
//-----------------------------------------------------------------------------
char* ISQueryLibPQ::ReadColumn(int Index)
{
	return PQgetvalue(SqlResult, CurrentRow, Index);
}
//-----------------------------------------------------------------------------
char* ISQueryLibPQ::ReadColumn(const std::string &FieldName)
{
	return ReadColumn(PQfnumber(SqlResult, FieldName.c_str()));
}
//-----------------------------------------------------------------------------
std::string ISQueryLibPQ::ReadColumn_String(int Index)
{
	char *Value = ReadColumn(Index);
	if (Value)
	{
		return Value;
	}
	return std::string();
}
//-----------------------------------------------------------------------------
std::string ISQueryLibPQ::ReadColumn_String(const std::string &FieldName)
{
	return ReadColumn_String(PQfnumber(SqlResult, FieldName.c_str()));
}
//-----------------------------------------------------------------------------
void ISQueryLibPQ::FillColumnMap()
{
	for (int i = 0; i < CountColumns; ++i)
	{
		ColumnMap[i] = ISSqlField
		{
			PQfname(SqlResult, i),
			PQftype(SqlResult, i)
		};
	}
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Prepare()
{
	return true;
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Prepare(PGconn *sql_connection)
{
	Q_UNUSED(sql_connection);
	return true;
}
//-----------------------------------------------------------------------------
