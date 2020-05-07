#include "ISTcpConnector.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	TcpSocket(new QTcpSocket(this))
{
	
}
//-----------------------------------------------------------------------------
ISTcpConnector::~ISTcpConnector()
{
	
}
//-----------------------------------------------------------------------------
QString ISTcpConnector::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QTcpSocket* ISTcpConnector::GetSocket()
{
	return TcpSocket;
}
//-----------------------------------------------------------------------------
ISTcpConnector& ISTcpConnector::Instance()
{
	static ISTcpConnector TcpConnector;
	return TcpConnector;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &host, quint16 port)
{
	TcpSocket->connectToHost(host, port);
	bool Result = TcpSocket->waitForConnected(CARAT_TIMEOUT_CONNECT);
	if (Result)
	{
		Result = TcpSocket->state() == QTcpSocket::ConnectedState;
	}
	
	if (!Result)
	{
		ErrorString = TcpSocket->errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
	if (TcpSocket->state() == QTcpSocket::ConnectedState)
	{
		TcpSocket->disconnect();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
	ErrorString = TcpSocket->errorString();
}
//-----------------------------------------------------------------------------