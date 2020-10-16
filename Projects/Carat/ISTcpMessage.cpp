#include "ISTcpMessage.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpMessage::ISTcpMessage(ISNamespace::ApiMessageType type, const QVariantMap &parameters, ISTcpSocket *tcp_socket)
	: Type(type),
	Parameters(parameters),
	TcpSocket(tcp_socket),
	ErrorString(NO_ERROR_STRING),
	Valid(true)
{

}
//-----------------------------------------------------------------------------
ISTcpMessage::ISTcpMessage(ISTcpSocket *tcp_socket, const QString &error_string)
	: Type(ISNamespace::AMT_Unknown),
	TcpSocket(tcp_socket),
	ErrorString(error_string),
	Valid(false)
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
bool ISTcpMessage::IsValid() const
{
	return Valid;
}
//-----------------------------------------------------------------------------
