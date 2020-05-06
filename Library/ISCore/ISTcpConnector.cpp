#include "ISTcpConnector.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
	: QObject(),
	ErrorString(NO_ERROR_STRING)
{
	TcpSocket = new QTcpSocket(this);
	connect(TcpSocket, &QTcpSocket::connected, &EventLoop, &QEventLoop::quit);
	connect(TcpSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), &EventLoop, &QEventLoop::quit);
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
	QTimer Timer;
	Timer.setSingleShot(true);
	connect(&Timer, &QTimer::timeout, [=] { TcpSocket->connectToHost(host, port); });
	Timer.start(50);
	EventLoop.exec();
	if (TcpSocket->state() == QTcpSocket::ConnectedState)
	{
		return true;
	}
	ErrorString = TcpSocket->errorString();
	return false;
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
