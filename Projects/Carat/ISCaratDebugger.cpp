#include "ISCaratDebugger.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISCaratDebugger::ISCaratDebugger(QObject *parent) : QObject(parent)
{
	Timer = new QTimer(this);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISCaratDebugger::TimeoutConnect);
	Timer->start(CARAT_DEBUGGER_CONNECT_TIMEOUT);

	LocalSocket = new QLocalSocket(this);
	connect(LocalSocket, &QLocalSocket::readyRead, this, &ISCaratDebugger::ReadyRead);
	connect(LocalSocket, &QLocalSocket::disconnected, this, &ISCaratDebugger::Disconnected);
	connect(LocalSocket, static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &ISCaratDebugger::Error);
	connect(LocalSocket, &QLocalSocket::stateChanged, this, &ISCaratDebugger::StateChanged);
}
//-----------------------------------------------------------------------------
ISCaratDebugger::~ISCaratDebugger()
{

}
//-----------------------------------------------------------------------------
void ISCaratDebugger::ReadyRead()
{
	ISLOGGER_L(LocalSocket->readAll());
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::Disconnected()
{
	ISLOGGER_L("Carat completed its work");
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::Error(QLocalSocket::LocalSocketError ErrorType)
{
    Q_UNUSED(ErrorType);
	ISLOGGER_L("Error connection: " + LocalSocket->errorString());
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::StateChanged(QLocalSocket::LocalSocketState NewState)
{
	if (NewState == QLocalSocket::UnconnectedState) //���� �� ������� ������������ - ������� ��� ���
	{
		Timer->start();
	}
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::Connect()
{
	LocalSocket->connectToServer(CARAT_DEBUGGER_PORT, QIODevice::ReadOnly);
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::TimeoutConnect()
{
	Connect();
}
//-----------------------------------------------------------------------------
