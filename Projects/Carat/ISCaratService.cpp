#include "ISCaratService.h"
#include "ISTcpServer.h"
#include "ISConstants.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent) : QObject(parent)
{
	
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{
	
}
//-----------------------------------------------------------------------------
bool ISCaratService::Start()
{
	ISTcpServer *TcpServer = new ISTcpServer(this);
	bool Result = TcpServer->Run(CARAT_DEFAULT_PORT);
	if (!Result)
	{
		ISLOGGER_W(QString("Not started TCP-server with port %1: %2").arg(CARAT_DEFAULT_PORT).arg(TcpServer->GetErrorString()));
	}
	return Result;
}
//-----------------------------------------------------------------------------
