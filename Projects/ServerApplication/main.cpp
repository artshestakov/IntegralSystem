#include "StdAfx.h"
#include "ISTcpServer.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	ISTcpServer *TcpServer = new ISTcpServer();
	if (!TcpServer->Run(CARAT_DEFAULT_PORT))
	{
		qDebug() << TcpServer->GetErrorString();
	}
	return Application.exec();
}
//-----------------------------------------------------------------------------
