#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer(ISTcpSocket *tcp_socket)
	: ErrorString(NO_ERROR_STRING),
	Error(false),
	TcpSocket(tcp_socket),
	SocketDescriptor(tcp_socket->GetSocketDescriptor())
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
QString ISTcpAnswer::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISTcpAnswer::SetError(const QString &error_string)
{
	if (!error_string.isEmpty())
	{
		Error = true;
		ErrorString = error_string;
	}
}
//-----------------------------------------------------------------------------
ISTcpSocket* ISTcpAnswer::GetSocket()
{
	return TcpSocket;
}
//-----------------------------------------------------------------------------
qintptr ISTcpAnswer::GetSocketDescriptor() const
{
	return SocketDescriptor;
}
//-----------------------------------------------------------------------------
QByteArray ISTcpAnswer::ToByteArray() const
{
	//Формируем ответ
	QByteArray ByteArray = ISSystem::VariantMapToJsonString(
	{
		{ "IsError", Error },
		{ "ErrorString", ErrorString },
		{ "Parameters", Parameters }
	}, QJsonDocument::Compact) + SYMBOL_NULL_TERM;
	ByteArray.insert(0, QByteArray::number(ByteArray.size()) + '.');
	return ByteArray;
}
//-----------------------------------------------------------------------------
