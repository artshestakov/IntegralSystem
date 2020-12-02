#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	TcpSocket(new QTcpSocket(this)),
	HandlingError(true)
{
	connect(TcpSocket, static_cast<void(QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error), this, &ISTcpConnector::Error);
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

	//Объявляем таумауты
	size_t Timeout = CARAT_CONNECT_TIMEOUT,
		SleepTime = CARAT_CONNECT_SLEEP;

	//Пытаемся подключиться и ждём пока не подключимся
	TcpSocket->connectToHost(Host, Port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
	while (!IsConnected())
	{
		Timeout -= SleepTime;
		ISSleep(SleepTime);
		PROCESS_EVENTS();
		if (!Timeout) //Таймаут истёк
		{
			TcpSocket->disconnectFromHost();
			ErrorString = LANG("TcpConnector.Timeout");
			break;
		}
	}

	if (!IsConnected())
	{
		ErrorString = TcpSocket->errorString();
		return false;
	}
	ErrorString = NO_ERROR_STRING;
	HandlingError = true; //Устанавливаем разрешение на обработку ошибок
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
	if (IsConnected()) //Если подключение активно - отключаемся
	{
		TcpSocket->disconnectFromHost();
	}
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
	if (HandlingError) //Если обработка ошибок включена - обрабатываем
	{
		ErrorString = TcpSocket->errorString();
		if (socket_error == QAbstractSocket::SocketError::RemoteHostClosedError) //Удалённый хост оборвал соединение - рапортуем о необходимости переподключения
		{
			QTimer::singleShot(100, this, &ISTcpConnector::Reconnect);
			HandlingError = false;
		}
	}
}
//-----------------------------------------------------------------------------
