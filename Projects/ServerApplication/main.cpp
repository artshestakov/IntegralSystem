#include "ISTcpServerCarat.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	ISTcpServerCarat TcpServer;
	if (!TcpServer.Run(CARAT_DEFAULT_PORT))
	{
		qDebug() << TcpServer.GetErrorString();
	}
	return Application.exec();
}
//-----------------------------------------------------------------------------
