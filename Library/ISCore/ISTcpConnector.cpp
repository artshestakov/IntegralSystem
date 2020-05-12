#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISCountingTime.h"
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
	Timer->setInterval(CARAT_TIMEOUT_CONNECT);
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
bool ISTcpConnector::Reconnect(const QString &host, quint16 port)
{
	if (IsConnected())
	{
		Disconnect();
	}
	return Connect(host, port);
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &host, quint16 port)
{
	Timer->start();
	TcpSocket->connectToHost(host, port);
	EventLoop.exec();
	return IsConnected();
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
void ISTcpConnector::StateChanged(QTcpSocket::SocketState socket_state)
{
	qDebug() << socket_state;
}
//-----------------------------------------------------------------------------
