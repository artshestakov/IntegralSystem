#include "ISTcpConnector.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	TcpSocket(new QTcpSocket(this))
{
	connect(TcpSocket, &QTcpSocket::connected, &EventLoop, &QEventLoop::quit);
	connect(TcpSocket, &QTcpSocket::stateChanged, this, &ISTcpConnector::StateChanged);
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
bool ISTcpConnector::Connect(const QString &Host, quint16 Port)
{
	TcpSocket->connectToHost(Host, Port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
	EventLoop.exec();
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
void ISTcpConnector::StateChanged(QAbstractSocket::SocketState socket_state)
{
	if (socket_state == QAbstractSocket::UnconnectedState) //Не удалось подключиться
	{
		ErrorString = TcpSocket->errorString();
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
