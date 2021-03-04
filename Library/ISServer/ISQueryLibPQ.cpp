#include "ISQueryLibPQ.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(const std::string &sql_text)
	: ErrorString(NO_ERROR_STRING),
	ShowLongQuery(true),
	SqlText(sql_text),
	ParameterCount(0),
	Prepared(false),
	SqlConnection(ISDatabase::Instance().GetDBLibPQ(CONNECTION_DEFAULT)),
	SqlResult(NULL),
	CountRows(-1),
	CountColumns(-1),
	CurrentRow(-1),
	IsSelect(false)
{
	
}
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text)
	: ErrorString(NO_ERROR_STRING),
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
ISQueryLibPQ::~ISQueryLibPQ()
{
	if (SqlResult)
	{
		PQclear(SqlResult);
	}
}
//-----------------------------------------------------------------------------
const std::string& ISQueryLibPQ::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
const std::string& ISQueryLibPQ::GetSqlText() const
{
	return SqlText;
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
void ISQueryLibPQ::BindValue(std::nullptr_t Pointer)
{
	IS_UNUSED(Pointer);
	ParameterValues.emplace_back(std::string());
	ParameterTypes.emplace_back(InvalidOid);
	++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQueryLibPQ::BindValue(int Value, Oid OID)
{
	ParameterValues.emplace_back(std::to_string(Value));
	ParameterTypes.emplace_back(OID == InvalidOid ? INT2OID : OID);
	++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQueryLibPQ::BindValue(unsigned int Value, Oid OID)
{
	ParameterValues.emplace_back(std::to_string(Value));
	ParameterTypes.emplace_back(OID == InvalidOid ? INT4OID : OID);
	++ParameterCount;
}
//-----------------------------------------------------------------------------
void ISQueryLibPQ::BindValue(const std::string &Value, Oid OID)
{
	ParameterValues.emplace_back(Value);
	ParameterTypes.emplace_back(OID == InvalidOid ? VARCHAROID : OID);
	++ParameterCount;
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::Execute(bool PrepareQuery, size_t ParamCount)
{
	if (PrepareQuery && !Prepared) //���� ������ ����� ����������� � �� ��� �� ����� - �������
	{
		if (!Prepare(ParamCount))
		{
			return false;
		}
	}

	//�������� ����� � ��������� ������
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	if (ParameterCount > 0) //���� ��������� ������� - ������������������ ������
	{
		//�������� ������ ���������� � �������� �������� ������ ��� ���
		char **ParamValues = (char **)malloc(ParameterCount * (sizeof(char *)));
		if (!ParamValues) //�� ������� �������� ������
		{
			ErrorString = "Malloc out of memory";
			return false;
		}

		//���������� ���������
		for (size_t i = 0; i < ParameterCount; ++i)
		{
			//�������� �������� ���������
			std::string String = ParameterValues[i];
			if (String.empty()) //���� �������� ������ - �������������� ��� ��� NULL
			{
				ParamValues[i] = nullptr;
			}
			else //�������� �� ������ - ����������� ���
			{
				//�������� ������ ��� ���������
				ParamValues[i] = (char *)malloc(String.size() + 1);
				if (!ParamValues[i]) //�� ������� �������� ������
				{
					free(ParamValues); //������� ������ ��� ������
					ErrorString = "Malloc out of memory";
					return false;
				}
				strcpy(ParamValues[i], String.c_str()); //�������� �������� � ������
			}
		}

		if (StmtName.empty()) //������ �� ��� ����������� - ��������� ���� � �����������, ���� ���
		{
			SqlResult = ParameterCount > 0 ? PQexecParams(SqlConnection, SqlText.c_str(), (int)ParameterCount, ParameterTypes.data(), ParamValues, NULL, NULL, 0) :
				PQexec(SqlConnection, SqlText.c_str());
		}
		else //������ ��� ����������� - ��������� �������������� ��������
		{
			SqlResult = PQexecPrepared(SqlConnection, StmtName.c_str(), (int)ParameterCount, ParamValues, NULL, NULL, 0);
		}

		//������� ���������� ������
		for (size_t i = 0; i < ParameterCount; ++i)
		{
			free(ParamValues[i]);
		}
		free(ParamValues);
		ParameterValues.clear();
		ParameterTypes.clear();
		ParameterCount = 0;
	}
	else //���������� ������� ��� - ��������� ��� ����
	{
		//���� ������ �� ��� ����������� - ��������� ��� ����, ����� - ��������� �������������� ��������
		SqlResult = StmtName.empty() ? PQexec(SqlConnection, SqlText.c_str()) :
			PQexecPrepared(SqlConnection, StmtName.c_str(), 0, NULL, NULL, NULL, 0);
	}

	//������� ����������� ����� �� ���������� �������
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
Oid ISQueryLibPQ::ColumnType(size_t Index)
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
char* ISQueryLibPQ::ReadColumn(size_t Index) const
{
	return PQgetvalue(SqlResult, CurrentRow, Index);
}
//-----------------------------------------------------------------------------
std::string ISQueryLibPQ::ReadColumn_String(size_t Index) const
{
	return ReadColumn(Index);
}
//-----------------------------------------------------------------------------
int ISQueryLibPQ::ReadColumn_Int(size_t Index) const
{
	return std::atoi(ReadColumn(Index));
}
//-----------------------------------------------------------------------------
unsigned int ISQueryLibPQ::ReadColumn_UInt(size_t Index) const
{
	return ReadColumn_Int(Index);
}
//-----------------------------------------------------------------------------
ISInt64 ISQueryLibPQ::ReadColumn_Int64(size_t Index) const
{
	return std::atoll(ReadColumn(Index));
}
//-----------------------------------------------------------------------------
ISUInt64 ISQueryLibPQ::ReadColumn_UInt64(size_t Index) const
{
	return ReadColumn_Int64(Index);
}
//-----------------------------------------------------------------------------
bool ISQueryLibPQ::ReadColumn_Bool(size_t Index) const
{
	return (strcmp(ReadColumn(Index), "t") == 0) ? true : false;
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
	PGresult *STMT = PQprepare(SqlConnection, StmtName.c_str(), SqlText.c_str(), ParamCount, ParameterTypes.data());
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
