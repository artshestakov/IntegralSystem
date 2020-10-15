#include "ISTcpAnswer.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer(ISTcpSocket *tcp_socket)
	: Error(false),
	TcpSocket(tcp_socket)
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
QByteArray ISTcpAnswer::ToByteArray() const
{
	//Формируем ответ
	QByteArray ByteArray = ISSystem::VariantMapToJsonString(
	{
		{ "IsError", Error },
		{ "ErrorString", ErrorString },
		{ "Parameters", Parameters }
	}, QJsonDocument::Compact);
	ByteArray.insert(0, QString("%1.").arg(ByteArray.size()));
	return ByteArray;
}
//-----------------------------------------------------------------------------
