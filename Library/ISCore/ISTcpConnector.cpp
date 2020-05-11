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
ISTcpConnector& ISTcpConnector::Instance()
{
    static ISTcpConnector TcpConnector;
    return TcpConnector;
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
bool ISTcpConnector::Connect(const QString &host, quint16 port)
{
	TcpSocket->connectToHost(QHostAddress(host), port);
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
		TcpSocket->disconnectFromHost();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
    Q_UNUSED(socket_error);
	ErrorString = TcpSocket->errorString();
}
//-----------------------------------------------------------------------------
