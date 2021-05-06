#include "ISTcpQuery.h"
#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLocalization.h"
#include "ISLogger.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <rapidjson/error/en.h>
//-----------------------------------------------------------------------------
ISTcpQuery::ISTcpQuery(const char *query_type)
    : ErrorString(STRING_NO_ERROR),
    QueryType(query_type),
    Parameters(rapidjson::Type::kObjectType),
    ParametersCount(0),
    AnswerObject(rapidjson::Type::kObjectType)
{

}
//-----------------------------------------------------------------------------
ISTcpQuery::~ISTcpQuery()
{

}
//-----------------------------------------------------------------------------
const std::string& ISTcpQuery::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
void ISTcpQuery::BindString(const std::string &ParamterName, const std::string &String)
{
    auto &Allocator = Parameters.GetAllocator();
    Parameters.AddMember(JSON_STRINGA(ParamterName.c_str(), Allocator), JSON_STRINGA(String.c_str(), Allocator), Allocator);
    ++ParametersCount;
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute(const char *query_type)
{
    QueryType = query_type;
    return ISTcpQuery::Execute();
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::Execute()
{
    //���������� ������� �����
    ISTimePoint TimePoint = ISAlgorithm::GetTick();

    //��������� JSON
    ISLOGGER_I(__CLASS__, "Build \"%s\"", QueryType);
    rapidjson::Document JsonObject(rapidjson::Type::kObjectType);
    auto &Allocator = JsonObject.GetAllocator();

    JsonObject.AddMember("Type", JSON_STRINGA(QueryType, Allocator), Allocator);

    if (ParametersCount > 0) //���� ���� ��������� - ��������� �� � JSON-������
    {
        JsonObject.AddMember("Parameters", Parameters.GetObject(), Allocator);
    }

    rapidjson::StringBuffer JsonBuffer;
    JsonBuffer.Clear();

    //��������� JSON-������ � ������
    rapidjson::Writer<rapidjson::StringBuffer> JsonWriter(JsonBuffer);
    JsonObject.Accept(JsonWriter);
    std::string StringBuffer = std::to_string(JsonBuffer.GetSize() + 1) + '.' + JsonBuffer.GetString() + CHAR_NULL_TERM;

    //��������� ������� ����������
    if (!CheckConnection())
    {
        return false;
    }

    //�������� ����� � ���������� �� ���� ������
    ISLOGGER_I(__CLASS__, "Sending...");
    ISSocket Socket = ISTcpConnector::Instance().GetSocket();
    int Result = send(Socket, StringBuffer.c_str(), (int)StringBuffer.size(), 0);
    if (Result == SOCKET_ERROR)
    {
        ErrorString = ISAlgorithm::StringF("Error sending: %s", ISTcpConnector::Instance().GetErrorString().c_str());
        return false;
    }
    ISLOGGER_I(__CLASS__, "Sended. Wait answer...");
    StringBuffer.clear();

    char Buffer[TCP_PACKET_MAX_SIZE] = { 0 }; //����� ��� ���������
    size_t AnswerSize = 0, CurrentSize = 0;
    while (true) //��� ���� �� ����� �����
    {
        ISSleep(1);
        Result = recv(Socket, Buffer, TCP_PACKET_MAX_SIZE, 0);

        //������ ��� �� �������� - ����������� ���
        if (AnswerSize == 0)
        {
            for (int i = 0; i < Result; ++i)
            {
                if (Buffer[i] == '.') //����� �����
                {
                    char Char[MAX_DIGIT_LENGHT] = { 0 };
                    std::memcpy(Char, Buffer, i); //�������� ������ �������
                    AnswerSize = std::atoi(Char); //�������� ������ � �������� � ������ �����
                    if (AnswerSize > 0) //���������� ������ �������
                    {
                        --Result -= i; //����������� ������ ������
                        std::memmove(Buffer, Buffer + i + 1, Result); //������� ����� (�������� ������ � �����)
                    }
                    break;
                }
            }

            if (AnswerSize == 0) //������ �� ������ - ��������� �� �������� - ������� � �������
            {
                ErrorString = "Error getting answer size";
                ISLOGGER_E(__CLASS__, ErrorString.c_str());
                return false;
            }
        }

        //��������� ������ ������ � ����� � ����������� ��� ������
        StringBuffer += std::string(Buffer, Result);
        CurrentSize += Result;

        if (AnswerSize == CurrentSize) //������ ������ ���������
        {
            ISLOGGER_I(__CLASS__, "Waited");
            break;
        }
    }

    //��������� ���������� ������
    bool ValueIsError = false;
    std::string ValueErrorString;
    if (IsValidAnswer(StringBuffer, AnswerSize, ValueIsError, ErrorString)) //����� �������
    {
        ISLOGGER_I(__CLASS__, "Validated answer");
        if (ValueIsError) //��������� ������ �� ������
        {
            ISLOGGER_E(__CLASS__, "Query failed: %s", ErrorString.c_str());
        }
        else
        {
            ISLOGGER_I(__CLASS__, "Query \"%s\" success (%d msec)", QueryType, ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint));
        }
    }
    else //����� ���������� - ������� ��������� ������ (����� ��� ���-�� ����)
    {
        ISLOGGER_E(__CLASS__, "Not validated answer: %s", ErrorString.c_str());
    }
    QueryType = nullptr;
    ParametersCount = 0;
    StringBuffer.clear(); //������� ��������� �����
    return !ValueIsError;
}
//-----------------------------------------------------------------------------
const rapidjson::Value& ISTcpQuery::GetAnswer() const
{
    return AnswerObject;
}
//-----------------------------------------------------------------------------
/*QVariant ISTcpQuery::GetParameter(const QString &ParameterName) const
{
    QVariant Value;
    if (Parameters.contains(ParameterName))
    {
        Value = Parameters[ParameterName];
    }
    return Value;
}*/
//-----------------------------------------------------------------------------
bool ISTcpQuery::IsValidAnswer(const std::string &StringBuffer, size_t Size, bool &ValueIsError, std::string &ValueErrorString)
{
    rapidjson::Document JsonDocument;
    rapidjson::ParseResult Result = JsonDocument.Parse(StringBuffer.c_str(), Size);
    if (Result.Code() != rapidjson::ParseErrorCode::kParseErrorNone)
    {
        ErrorString = GetParseError_En(Result.Code());
        return false;
    }

    if (!JsonDocument.HasMember("IsError")) //���� ���� � ������ ������ �����������
    {
        ErrorString = "Not found field \"IsError\"";
        return false;
    }

    rapidjson::Value &JsonValue = JsonDocument["IsError"];
    if (!JsonValue.IsBool()) //�������� ���
    {
        ErrorString = "Invalid type field \"IsError\"";
        return false;
    }

    ValueIsError = JsonValue.GetBool();
    if (ValueIsError) //���� ������ ������������� ����, ������ ����� ��������� ������� � ��������
    {
        if (!JsonDocument.HasMember("ErrorString")) //���� ���� � ��������� ������ �����������
        {
            ErrorString = "Not found field \"ErrorString\"";
            return false;
        }

        JsonValue = JsonDocument["ErrorString"];
        if (!JsonValue.IsString()) //���� ���� � ��������� ������ ������
        {
            ErrorString = "Invalid type field \"ErrorString\"";
            return false;
        }
        ValueErrorString = JsonValue.GetString();
    }
    else
    {
        AnswerObject.CopyFrom(JsonDocument["Parameters"], AnswerObject.GetAllocator());
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpQuery::CheckConnection()
{
    if (!ISTcpConnector::Instance().IsConnected())
    {
        ISLOGGER_E(__CLASS__, "Not connected to host");
        ErrorString = LANG("NotConnectToServer");
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*ISTcpQueryTable::ISTcpQueryTable() : ISTcpQuery(API_GET_TABLE_DATA)
{

}
//-----------------------------------------------------------------------------
ISTcpQueryTable::~ISTcpQueryTable()
{

}
//-----------------------------------------------------------------------------
bool ISTcpQueryTable::Execute()
{
    if (!FilterMap.isEmpty())
    {
        BindValue("Filter", FilterMap);
    }

    if (!SortingMap.isEmpty())
    {
        BindValue("Sorting", SortingMap);
    }
    return ISTcpQuery::Execute();
}
//-----------------------------------------------------------------------------
void ISTcpQueryTable::SetSorting(const QString &SortingField, Qt::SortOrder SortingOrder)
{
    SortingMap = QVariantMap
    {
        { "Field", SortingField },
        { "Order", SortingOrder }
    };
}
//-----------------------------------------------------------------------------
void ISTcpQueryTable::SetSearch(const QVariantList &VariantMap)
{
    BindValue("Search", VariantMap);
}
//-----------------------------------------------------------------------------
void ISTcpQueryTable::AddFilter(const QString &FieldName, const QVariant &Value)
{
    FilterMap[FieldName] = Value;
}
//-----------------------------------------------------------------------------
*/