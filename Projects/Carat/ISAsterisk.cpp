#include "ISAsterisk.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISAsterisk::ISAsterisk(QObject *parent)
	: QThread(parent),
	Port(0)
{

}
//-----------------------------------------------------------------------------
ISAsterisk::~ISAsterisk()
{

}
//-----------------------------------------------------------------------------
bool ISAsterisk::Start()
{
	Host = CONFIG_STRING(CONST_CONFIG_AMI_SERVER);
	if (Host.isEmpty())
	{
		ISLOGGER_E(__CLASS__, QString("Not specified config parameter: %1").arg(CONST_CONFIG_AMI_SERVER));
		return false;
	}

	Port = CONFIG_INT(CONST_CONFIG_AMI_PORT);
	if (Port < 1 || Port >= USHRT_MAX) //≈сли значение не входит в диапазон портов - использует порт по умолчанию
	{
		ISLOGGER_E(__CLASS__, QString("Invalid config value %1: %2.").arg(CONST_CONFIG_AMI_PORT).arg(Port));
		return false;
	}

	Login = CONFIG_STRING(CONST_CONFIG_AMI_LOGIN);
	if (Login.isEmpty())
	{
		ISLOGGER_E(__CLASS__, QString("Not specified config parameter: %1").arg(CONST_CONFIG_AMI_LOGIN));
		return false;
	}

	Password = CONFIG_STRING(CONST_CONFIG_AMI_PASSWORD);
	if (Password.isEmpty())
	{
		ISLOGGER_E(__CLASS__, QString("Not specified config parameter: %1").arg(CONST_CONFIG_AMI_PASSWORD));
		return false;
	}

	start();
	return true;
}
//-----------------------------------------------------------------------------
void ISAsterisk::run()
{
	TcpSocket = new QTcpSocket(this);
	connect(TcpSocket, &QTcpSocket::connected, this, &ISAsterisk::Connected);
	connect(TcpSocket, &QTcpSocket::disconnected, this, &ISAsterisk::Disconnected);
	connect(TcpSocket, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &ISAsterisk::Error);
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISAsterisk::ReadyRead);
	TcpSocket->connectToHost(Host, Port);
	exec();
}
//-----------------------------------------------------------------------------
void ISAsterisk::Connected()
{
	ISLOGGER_I(__CLASS__, "Connected to AMI");
}
//-----------------------------------------------------------------------------
void ISAsterisk::Disconnected()
{

}
//-----------------------------------------------------------------------------
void ISAsterisk::Error(QAbstractSocket::SocketError socket_error)
{
	Q_UNUSED(socket_error);
	ISLOGGER_E(__CLASS__, TcpSocket->errorString());
}
//-----------------------------------------------------------------------------
void ISAsterisk::ReadyRead()
{
	Buffer.push_back(TcpSocket->readAll());
}
//-----------------------------------------------------------------------------
