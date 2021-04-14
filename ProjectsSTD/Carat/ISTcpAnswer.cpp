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
std::string ISTcpAnswer::GetErrorString() const
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
    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &Allocator = JsonDocument.GetAllocator();

    JsonDocument.AddMember("IsError", Error, Allocator);
    JsonDocument.AddMember("ErrorString", JSON_STRING(ErrorString.c_str()), Allocator);

    //Если ошибки нет и есть параметры - добавляем их в ответ
    if (!Error && !Parameters.ObjectEmpty())
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
