#include "ISTcpMessage.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISTcpMessage::ISTcpMessage()
	: ErrorString(STRING_NO_ERROR),
	Type(API_UNKNOWN),
	MSecParse(0),
	Size(0),
	ChunkCount(0),
	Valid(true)
{

}
//-----------------------------------------------------------------------------
ISTcpMessage::~ISTcpMessage()
{

}
//-----------------------------------------------------------------------------
std::string ISTcpMessage::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISTcpMessage::SetErrorString(const std::string &error_string)
{
	Valid = true;
	ErrorString = error_string;
	ISLOGGER_E(__CLASS__, error_string);
}
//-----------------------------------------------------------------------------
bool ISTcpMessage::IsValid() const
{
	return Valid;
}
//-----------------------------------------------------------------------------
