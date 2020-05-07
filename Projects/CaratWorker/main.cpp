#include "ISTcpServerWorker.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	ISTcpServerWorker TcpServerWorker;
	bool Result = TcpServerWorker.Run(CARAT_DEFAULT_PORT);
	if (!Result)
	{
		qDebug() << TcpServerWorker.GetErrorString();
	}
	return Result ? Application.exec() : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
