#include "StdAfx.h"
#include "ISTcpServer.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	ISTcpServer *TcpServer = new ISTcpServer(nullptr);
	if (!TcpServer->Run(23071))
	{
		qDebug() << TcpServer->GetErrorString();
	}
	return Application.exec();
}
//-----------------------------------------------------------------------------
