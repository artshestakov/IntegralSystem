#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer(ISSocket socket_client)
    : Parameters(rapidjson::Type::kObjectType),
    ErrorString(STRING_NO_ERROR),
    Error(false),
    SocketClient(socket_client)
{

}
//-----------------------------------------------------------------------------
ISTcpAnswer::~ISTcpAnswer()
{

}
//-----------------------------------------------------------------------------
bool ISTcpAnswer::IsError() const
{
    return Error;
}
//-----------------------------------------------------------------------------
const std::string& ISTcpAnswer::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
void ISTcpAnswer::SetError(const std::string &error_string)
{
    Error = true;
    ErrorString = error_string;
}
//-----------------------------------------------------------------------------
ISSocket ISTcpAnswer::GetSocketClient() const
{
    return SocketClient;
}
//-----------------------------------------------------------------------------
std::string ISTcpAnswer::ToJson() const
{
    rapidjson::Document JsonDocument(rapidjson::Type::kObjectType);
    auto &Allocator = JsonDocument.GetAllocator();

    JsonDocument.AddMember("IsError", Error, Allocator);

    if (Error) //Ошибка есть добавим её описание в ответ
    {
        JsonDocument.AddMember("ErrorString", JSON_STRINGA(ErrorString.c_str(), Allocator), Allocator);
    }
    else if (!Parameters.ObjectEmpty()) //Ошибки нет, если параметры в ответе есть - добавим их в ответ
    {
        JsonDocument.AddMember("Parameters", rapidjson::Value(Parameters, Allocator), Allocator);
    }

    rapidjson::StringBuffer JsonBuffer;
    JsonBuffer.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> JsonWriter(JsonBuffer);
    JsonDocument.Accept(JsonWriter);
    return std::to_string(JsonBuffer.GetSize() + 1) + '.' + JsonBuffer.GetString() + CHAR_NULL_TERM;
}
//-----------------------------------------------------------------------------
