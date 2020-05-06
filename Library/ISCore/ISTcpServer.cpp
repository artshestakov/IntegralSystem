#include "ISTcpServer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer(QObject *parent)
	: QObject(parent),
	ErrorString(NO_ERROR_STRING),
	TcpServer(nullptr)
{
	
}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{

}
//-----------------------------------------------------------------------------
QString ISTcpServer::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpServer::Run(quint16 Port)
{
	TcpServer = new QTcpServer(this);
	bool Result = TcpServer->listen(QHostAddress::AnyIPv4, Port);
	if (Result)
	{
		connect(TcpServer, &QTcpServer::newConnection, this, &ISTcpServer::NewConnection);
		//if (TcpServer->waitForNewConnection(30000))
		{
			//QTcpSocket *s = TcpServer->nextPendingConnection();
			//s = s;
		}
		connect(TcpServer, &QTcpServer::acceptError, this, &ISTcpServer::AcceptError);
	}
	else
	{
		ErrorString = TcpServer->errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpServer::NewConnection()
{
	if (TcpServer->hasPendingConnections())
	{
		QTcpSocket *TcpSocket = TcpServer->nextPendingConnection();
		qintptr d = TcpSocket->socketDescriptor();
		d = d;
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::AcceptError(QAbstractSocket::SocketError socket_error)
{
	ErrorString = TcpServer->errorString();
}
//-----------------------------------------------------------------------------
void ISTcpServer::Disconnect()
{

}
//-----------------------------------------------------------------------------
