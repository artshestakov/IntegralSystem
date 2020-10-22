#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISTcp.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	TcpSocket(new QTcpSocket(this)),
	Timer(new QTimer(this))
{
	connect(TcpSocket, &QTcpSocket::connected, &EventLoop, &QEventLoop::quit);
	connect(TcpSocket, &QTcpSocket::connected, Timer, &QTimer::stop);

	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISTcpConnector::Timeout);
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
bool ISTcpConnector::IsConnected() const
{
	return TcpSocket->state() == QTcpSocket::ConnectedState;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Reconnect(const QString &Host, quint16 Port)
{
	if (IsConnected())
	{
		Disconnect();
	}
	return Connect(Host, Port);
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &Host, quint16 Port)
{
	//Timer->start();
	TcpSocket->connectToHost(Host, Port);
	//EventLoop.exec();
	return IsConnected();
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
	if (IsConnected())
	{
		TcpSocket->disconnectFromHost();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Timeout()
{
	//Подключение не произошло - ошибка
	if (TcpSocket->state() != QTcpSocket::ConnectedState)
	{
		ErrorString = "Connecting timeout";
		EventLoop.quit();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ErrorString = TcpSocket->errorString();
	EventLoop.quit();
}
//-----------------------------------------------------------------------------
