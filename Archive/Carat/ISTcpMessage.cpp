#include "ISTcpMessage.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpMessage::ISTcpMessage(ISTcpSocket *tcp_socket)
	: Type(API_UNKNOWN),
	TcpSocket(tcp_socket),
    ParseMSec(0),
    Size(0),
    ChunkCount(0),
	MessageID(0),
	ErrorString(NO_ERROR_STRING),
    Valid(true)
{

}
//-----------------------------------------------------------------------------
ISTcpMessage::~ISTcpMessage()
{

}
//-----------------------------------------------------------------------------
QString ISTcpMessage::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISTcpMessage::SetErrorString(const QString &error_string)
{
	Valid = false;
	ErrorString = error_string;
}
//-----------------------------------------------------------------------------
bool ISTcpMessage::IsValid() const
{
	return Valid;
}
//-----------------------------------------------------------------------------
