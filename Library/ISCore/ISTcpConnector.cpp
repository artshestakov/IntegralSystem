#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLocalization.h"
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
bool ISTcpConnector::IsConnected() const
{
	return TcpSocket->state() == QTcpSocket::ConnectedState;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &Host, quint16 Port)
{
	if (IsConnected()) //Если подключение поднято - возвращаем true
	{
		return true;
	}

	//Пытаемся подключиться
	TcpSocket->connectToHost(Host, Port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

	size_t Timeout = CARAT_CONNECT_TIMEOUT,
		SleepTime = CARAT_CONNECT_SLEEP;
	while (!IsConnected()) //Ждём пока не подключимся
	{
		Timeout -= SleepTime;
		ISSleep(SleepTime);
		ISSystem::ProcessEvents();
		if (!Timeout) //Таймаут истёк
		{
			TcpSocket->disconnectFromHost();
			ErrorString = LANG("TcpConnector.Timeout");
			break;
		}
	}

	bool Result = IsConnected();
	if (Result)
	{
		connect(TcpSocket, &QTcpSocket::stateChanged, this, &ISTcpConnector::StateChanged);
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
	if (IsConnected())
	{
		disconnect(TcpSocket, &QTcpSocket::stateChanged, this, &ISTcpConnector::StateChanged);
		TcpSocket->disconnectFromHost();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::StateChanged(QAbstractSocket::SocketState socket_state)
{
	if (socket_state == QAbstractSocket::UnconnectedState) //Соединение отвалилось - предлагаем переподключиться
	{
		ErrorString = TcpSocket->errorString();
		emit RemoteHostClose();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ErrorString = TcpSocket->errorString();
}
//-----------------------------------------------------------------------------
