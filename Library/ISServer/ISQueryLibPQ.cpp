#include "ISQueryLibPQ.h"
#include "ISConstants.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(const QString &sql_text, bool prepare)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	SqlConnection(ISDatabase::Instance().GetDBLibPQ(CONNECTION_DEFAULT)),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1)
{
	if (prepare && !sql_text.isEmpty())
	{
		Prepare();
	}
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(PGconn *sql_connection, const QString &sql_text, bool prepare)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	SqlConnection(sql_connection),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1)
{
	if (prepare && !sql_text.isEmpty())
	{
		Prepare(sql_connection);
	}
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::~ISQueryLibPQ()
{

}
//-----------------------------------------------------------------------------
QString ISQueryLibPQ::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
int ISQueryLibPQ::GetResultSize() const
{
	return CountRows;
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
	SqlResult = PQexec(SqlConnection, SqlText.toStdString().c_str());
	if (!SqlResult) //Не удалось отправить запрос
	{
		ErrorString = "Out of memory";
		return false;
	}

	switch (PQresultStatus(SqlResult)) //Проверяем результат
	{
	case PGRES_TUPLES_OK:
		CountRows = PQntuples(SqlResult);
		CountColumns = PQnfields(SqlResult);
		return true;
		break;

	case PGRES_SINGLE_TUPLE:
		return true;
		break;

	case PGRES_COMMAND_OK:
		return true;
		break;

	default:
		break;
	}

	//Получаем текст ошибки и выходим
	ErrorString = QString::fromLocal8Bit(PQerrorMessage(SqlConnection));
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
QString ISQueryLibPQ::ReadColumn_String(const QString &FieldName)
{
	Q_UNUSED(FieldName);
	char *Value = PQgetvalue(SqlResult, CurrentRow, 0);
	return QString::fromLocal8Bit(Value);
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
