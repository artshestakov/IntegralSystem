#include "ISTcpServerWorker.h"
#include "ISCore.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	
	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

	Result = argc == 2;
	if (Result) //Порт указан
	{
		int Port = std::atoi(argv[1]);
		Result = Port > 0;
		if (Result) //Порт валидный
		{
			ISTcpServerWorker TcpServerWorker;
			Result = TcpServerWorker.Run(CARAT_DEFAULT_PORT);
			if (!Result) //Не удалось запустить сервер
			{
				ISLOGGER_E(TcpServerWorker.GetErrorString());
			}
		}
		else //Порт не валидный
		{
			ISLOGGER_E(QString("Invalid port %1").arg(Port));
		}
	}
	else //Порт не указан
	{
		ISLOGGER_E("Port not specified");
	}
	return Result ? Application.exec() : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
