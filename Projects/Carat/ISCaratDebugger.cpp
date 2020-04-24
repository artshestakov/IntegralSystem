#include "ISCaratDebugger.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISCaratDebugger::ISCaratDebugger(QObject *parent) : QObject(parent)
{
	Timer = new QTimer(this);
	Timer->setInterval(CARAT_DEBUGGER_CONNECT_TIMEOUT);
	Timer->setSingleShot(true);
	connect(Timer, &QTimer::timeout, this, &ISCaratDebugger::TimeoutConnect);
	Timer->start();

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
	printf("%s\n", LocalSocket->readAll().constData());
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::Disconnected()
{
	printf("Carat completed its work\n");
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::Error(QLocalSocket::LocalSocketError ErrorType)
{
    Q_UNUSED(ErrorType);
	printf("Error connection: %s\n", LocalSocket->errorString().toStdString().c_str());
}
//-----------------------------------------------------------------------------
void ISCaratDebugger::StateChanged(QLocalSocket::LocalSocketState NewState)
{
	if (NewState == QLocalSocket::UnconnectedState) //Если не удалось подключиться - пробуем ещё раз
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
