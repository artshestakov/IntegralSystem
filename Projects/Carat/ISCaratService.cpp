#include "ISCaratService.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent)
	: QObject(parent),
	LocalSocket(new QLocalSocket(this)),
	IsConnectedDebugger(false)
{
	
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{
	
}
//-----------------------------------------------------------------------------
bool ISCaratService::StartService()
{
	//«апуск локального сервера дл€ отладки
	LocalServer = new QLocalServer(this);
	LocalServer->setMaxPendingConnections(CARAT_DEBUGGER_MAX_CLIENTS);
	bool Result = LocalServer->listen(CARAT_DEBUGGER_PORT);
	if (Result)
	{
		connect(LocalServer, &QLocalServer::newConnection, this, &ISCaratService::NewConnection);
	}
	else
	{
        ISLOGGER_W("Error listen port for local server: " + LocalServer->errorString());
	}

	if (Result)
	{
		TcpServer = new ISTcpServerCarat(this);
		Result = TcpServer->Run(CARAT_DEFAULT_PORT);
		if (!Result)
		{
			ISLOGGER_W(QString("Not started TCP-server with port %1: %2").arg(CARAT_DEFAULT_PORT).arg(TcpServer->GetErrorString()));
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCaratService::NewConnection()
{
	LocalSocket = LocalServer->nextPendingConnection();
	connect(LocalSocket, &QLocalSocket::disconnected, this, &ISCaratService::Disconnected);
	IsConnectedDebugger = true;
}
//-----------------------------------------------------------------------------
void ISCaratService::Disconnected()
{
	LocalSocket->deleteLater();
	IsConnectedDebugger = false;
}
//-----------------------------------------------------------------------------
