#include "ISQueryLibPQ.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryLibPQ::ISQueryLibPQ(const std::string &sql_text, bool prepare, int ParamCount)
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
ISQueryLibPQ::ISQueryLibPQ(PGconn *sql_connection, const std::string &sql_text, bool prepare, int ParamCount)
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
void ISQueryLibPQ::AddBindValue(const ISVariant &Value)
{
	Parameters.emplace_back(Value);
	Oid OidType = NULL;
	switch (Value.GetType()) //���������� ��� ���������
	{
	case ISNamespace::VariantType::Bool:	OidType = BOOLOID;		break;
	case ISNamespace::VariantType::Short:	OidType = INT2OID;		break;
	case ISNamespace::VariantType::Int:		OidType = INT4OID;		break;
	case ISNamespace::VariantType::Int64:	OidType = INT8OID;		break;
	case ISNamespace::VariantType::UInt:	OidType = INT4OID;		break;
	case ISNamespace::VariantType::Double:	OidType = NUMERICOID;	break;
	case ISNamespace::VariantType::Float:	OidType = FLOAT4OID;	break;
	case ISNamespace::VariantType::Char:	OidType = CHAROID;		break;
	case ISNamespace::VariantType::String:	OidType = VARCHAROID;	break;
	case ISNamespace::VariantType::Uuid:	OidType = UUIDOID;		break;
	default:
		break;
	}
	Types.emplace_back(OidType);
	++ParametersCount; //�������������� �������� ���������
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
		//�������� ������ ���������� � �������� �������� ������ ��� ���
		char **ParamValues = (char **)malloc(ParametersCount * (sizeof(char *)));
		if (!ParamValues) //�� ������� �������� ������
		{
			ErrorString = "Malloc out of memory";
			return false;
		}

		//�������� ������ ��� ���� ����������
		Oid *ParamTypes = (Oid *)malloc(ParametersCount * sizeof(Oid *));

		//���������� ���������
		for (size_t i = 0; i < ParametersCount; ++i)
		{
			//�������� �������� ��������� � �������� ��� � ������
			ISVariant Value = Parameters[i];
			std::string String = Value.ToString();

			//�������� ������ ��� ���������
			ParamValues[i] = (char *)malloc(String.size() + 1);
			if (!ParamValues[i]) //�� ������� �������� ������
			{
				free(ParamValues); //������� ������ ��� ������
				ErrorString = "Malloc out of memory";
				return false;
			}
			strcpy(ParamValues[i], String.c_str()); //�������� �������� � ������
			ParamTypes[i] = Types[i];
		}

		if (StmtName.empty()) //������ �� ��� ����������� - ��������� ���� � �����������, ���� ���
		{
			SqlResult = ParametersCount > 0 ? PQexecParams(SqlConnection, SqlText.c_str(), (int)ParametersCount, NULL, ParamValues, NULL, NULL, 0) :
				PQexec(SqlConnection, SqlText.c_str());
		}
		else //������ ��� ����������� - ��������� �������������� ��������
		{
			SqlResult = PQexecPrepared(SqlConnection, StmtName.c_str(), (int)ParametersCount, ParamValues, NULL, NULL, 0);
		}

		//������� ���������� ������
		for (size_t i = 0; i < ParametersCount; ++i)
		{
			free(ParamValues[i]);
		}
		free(ParamValues);
		free(ParamTypes);
		Parameters.clear();
		ParametersCount = 0;
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
ISVariant ISQueryLibPQ::ReadColumn(int Index)
{
	ISVariant Value;
	if (PQgetisnull(SqlResult, CurrentRow, Index)) //�������� ����������� - ���������� ����������
	{
		return Value;
	}

	//�������� ��������, ��������� ��� ��� � ����������
	char *Char = PQgetvalue(SqlResult, CurrentRow, Index);
	switch (ColumnMap[Index].Type)
	{
	case BOOLOID:
		Value.SetBool(strcmp(Char, "t") == 0);
		break;

	case INT2OID:
		Value.SetShort((short)atoi(Char));
		break;

	case INT4OID:
		Value.SetInt(atoi(Char));
		break;

	case INT8OID:
		Value.SetInt64(_atoi64(Char));
		break;

	case NUMERICOID:
		Value.SetDouble(atof(Char));
		break;

	case FLOAT4OID:
		Value.SetFloat((float)atof(Char));
		break;

	case CHAROID:
		Value.SetChar(Char[0]);
		break;

	case VARCHAROID:
		Value.SetString(Char);
		break;
	}
	return Value;
}
//-----------------------------------------------------------------------------
ISVariant ISQueryLibPQ::ReadColumn(const std::string &FieldName)
{
	return ReadColumn(PQfnumber(SqlResult, FieldName.c_str()));
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
bool ISQueryLibPQ::Prepare(int ParamCount)
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
bool ISQueryLibPQ::Prepare(PGconn *sql_connection, int ParamCount)
{
	SqlConnection = sql_connection;
	return Prepare(ParamCount);
}
//-----------------------------------------------------------------------------
