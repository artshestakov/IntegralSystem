#include "ISTcpServerBase.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTcpServerBase::ISTcpServerBase(QObject *parent)
	: QTcpServer(parent),
	ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISTcpServerBase::~ISTcpServerBase()
{

}
//-----------------------------------------------------------------------------
QString ISTcpServerBase::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpServerBase::Run(qint16 Port)
{
	bool Result = listen(QHostAddress::AnyIPv4, Port);
	if (Result)
	{
		connect(this, &ISTcpServerBase::acceptError, this, &ISTcpServerBase::AcceptError);
	}
	else
	{
		ErrorString = errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::Send(QTcpSocket *TcpSocket, const QVariantMap &Data)
{
	//ќтправка данных
	TcpSocket->write(ISSystem::VariantMapToJsonString(Data).simplified().toUtf8() + CARAT_PACKET_SEPARATOR);
	TcpSocket->flush();

	//∆дЄм пока данные уйдут
	TcpSocket->waitForBytesWritten();
}
//-----------------------------------------------------------------------------
void ISTcpServerBase::AcceptError(QAbstractSocket::SocketError)
{
	ErrorString = errorString();
}
//-----------------------------------------------------------------------------
