#include "ISTcpServerWorker.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConfig.h"
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

	Result = argc == 4;
	if (Result) //Порт указан, логин и пароль указаны
	{
		int Port = std::atoi(argv[1]);
		Result = Port > 0;
		if (Result) //Порт валидный
		{
			Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), argv[2], argv[3]);
			if (!Result)
			{
				ISLOGGER_E(ISDatabase::Instance().GetErrorString());
				return EXIT_FAILURE;
			}

			ISTcpServerWorker TcpServerWorker;
			Result = TcpServerWorker.Run(static_cast<quint16>(Port));
			if (Result) //Не удалось запустить сервер
			{
				Result = Application.exec() == EXIT_SUCCESS ? true : false;
			}
			else
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
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
