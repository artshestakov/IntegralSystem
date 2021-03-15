#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer(SOCKET socket)
	: ErrorString(STRING_NO_ERROR),
	Error(false),
	Socket(socket)
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
SOCKET ISTcpAnswer::GetSocket() const
{
	return Socket;
}
//-----------------------------------------------------------------------------
std::string ISTcpAnswer::ToJson() const
{
	rapidjson::Document JsonDocument;
	rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &Allocator = JsonDocument.GetAllocator();

	JsonDocument.SetObject();
	JsonDocument.AddMember("Error", rapidjson::Value().SetBool(Error), Allocator);
	JsonDocument.AddMember("ErrorString", rapidjson::Value().SetString(ErrorString.c_str(), (rapidjson::SizeType)ErrorString.size(), Allocator), Allocator);
	
	rapidjson::StringBuffer JsonBuffer;
	JsonBuffer.Clear();

	rapidjson::Writer<rapidjson::StringBuffer> JsonWriter(JsonBuffer);
	JsonDocument.Accept(JsonWriter);
	return std::to_string(JsonBuffer.GetSize()) + '.' + JsonBuffer.GetString() + SYMBOL_NULL_TERM;
}
//-----------------------------------------------------------------------------
