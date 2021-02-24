#include "ISQueryLibPQ.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(const std::string &sql_text, bool prepare, size_t ParamCount)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	ParametersCount(0),
	Prepared(false),
	SqlConnection(ISDatabase::Instance().GetDBLibPQ(CONNECTION_DEFAULT)),
	SqlResult(NULL),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1),
	IsSelect(false)
{
	if (prepare && !sql_text.empty())
	{
		Prepare(ParamCount);
	}
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text, bool prepare, size_t ParamCount)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	ParametersCount(0),
	Prepared(false),
	SqlConnection(sql_connection),
	SqlResult(NULL),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1),
	IsSelect(false)
{
	if (prepare && !sql_text.empty())
	{
		Prepare(sql_connection, ParamCount);
	}
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::~ISQueryLibPQ()
{
	for (size_t i = 0; i < ParametersCount; ++i)
	{
		free(Parameters[i]);
	}
	Parameters.clear();

	if (SqlResult)
	{
		PQclear(SqlResult);
	}
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
void ISQueryLibPQ::AddBindValue(char *Value)
{
	char *Char = (char *)malloc(strlen(Value) + 1);
	strcpy(Char, Value);
	Parameters.emplace_back(Char);
	++ParametersCount;
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Execute()
{
	if (!StmtName.empty() && !Prepared) //���� ������� ����������� ��������, �� ��� �� ����������� �������
	{
		return false;
	}

	//�������� ����� � ��������� ������
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	if (Parameters.empty()) //���������� ������� ��� - ��������� ��� ����
	{
		//���� ������ �� ��� ����������� - ��������� ��� ����, ����� - ��������� �������������� ��������
		SqlResult = StmtName.empty() ? PQexec(SqlConnection, SqlText.c_str()) :
			PQexecPrepared(SqlConnection, StmtName.c_str(), 0, NULL, NULL, NULL, 0);
	}
	else //���� ��������� ������� - ������������������ ������
	{
		//�������� ������ ���������� � �������� �������� ������
		char **ParamValues = (char **)malloc(ParametersCount * (sizeof(char *)));
		if (!ParamValues) //�� ������� �������� ������
		{
			ErrorString = "Malloc out of memory";
			return false;
		}

		//���������� ���������
		for (size_t i = 0; i < ParametersCount; ++i)
		{
			ParamValues[i] = (char *)malloc(strlen(Parameters[i]) + 1); //�������� ������ ��� ���������
			if (!ParamValues[i]) //�� ������� �������� ������
			{
				free(ParamValues); //������� ������ ��� ������
				ErrorString = "Malloc out of memory";
				return false;
			}
			strcpy(ParamValues[i], Parameters[i]); //�������� �������� � ������
		}

		//��������� ������
		//���� ������ �� ��� ����������� - ��������� ��� ����, ����� - ��������� �������������� ��������
		SqlResult = StmtName.empty() ? PQexec(SqlConnection, SqlText.c_str()) :
			PQexecPrepared(SqlConnection, StmtName.c_str(), 0, NULL, NULL, NULL, 0);

		//������� ���������� ������ ��� ���������
		for (size_t i = 0; i < ParametersCount; ++i)
		{
			free(ParamValues[i]);
		}
		free(ParamValues);
	}
	long long Msec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
	if (!SqlResult) //�� ������� ��������� ������
	{
		ErrorString = "Out of memory";
		return false;
	}

	//���� ������ ���������� ������ MAX_QUERY_TIME - ������� ��������������
	if (ShowLongQuery && Msec > MAX_QUERY_TIME)
	{
		ISLOGGER_W(__CLASS__, QString("Long query msec: %1. %2").arg(Msec).arg(SqlText.c_str()));
	}

	switch (PQresultStatus(SqlResult)) //��������� ���������
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

	//�������� ����� ������ � �������
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
bool ISQueryLibPQ::Prepare(size_t ParamCount)
{
	StmtName = ISAlgorithm::StringToMD5(SqlText);
	PGresult *STMT = PQprepare(SqlConnection, StmtName.c_str(), SqlText.c_str(), ParamCount, NULL);
	Prepared = STMT ? true : false;
	if (Prepared)
	{
		//��������� ���������, ������� ������ � ��������� ���������
		ExecStatusType ResultStatus = PQresultStatus(STMT);
		PQclear(STMT);
		Prepared = ResultStatus == PGRES_TUPLES_OK || ResultStatus == PGRES_SINGLE_TUPLE || ResultStatus == PGRES_COMMAND_OK;
		if (!Prepared) //���������� �� �������
		{
			ErrorString = PQerrorMessage(SqlConnection);
		}
	}
	else
	{
		ErrorString = "Out of memory";
	}
	return Prepared;
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Prepare(PGconn *sql_connection, size_t ParamCount)
{
	SqlConnection = sql_connection;
	return Prepare(ParamCount);
}
//-----------------------------------------------------------------------------
