#include "ISCaratService.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent) : QObject(parent)
{
	
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{
	
}
//-----------------------------------------------------------------------------
bool ISCaratService::StartService()
{
	TcpServer = new ISTcpServerCarat(this);
	bool Result = TcpServer->Run(CARAT_DEFAULT_PORT);
	if (!Result)
	{
		ISLOGGER_W(QString("Not started TCP-server with port %1: %2").arg(CARAT_DEFAULT_PORT).arg(TcpServer->GetErrorString()));
	}
	return Result;
}
//-----------------------------------------------------------------------------
