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
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

	Result = argc == 5;
	if (Result) //Порт указан, логин и пароль указаны, токен тоже указан
	{
		int Port = std::atoi(argv[1]);
		Result = Port > 0;
		if (Result) //Порт валидный
		{
			Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
				CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
				argv[2], argv[3]);
			if (!Result) //Не удалось подключиться к БД
			{
				ISLOGGER_E(ISDatabase::Instance().GetErrorString());
				return EXIT_FAILURE;
			}

			ISTcpServerWorker TcpServerWorker;
			TcpServerWorker.SetToken(argv[4]);
			Result = TcpServerWorker.Run(static_cast<quint16>(Port));
			if (Result) //Сервер запущен - сообщаем об этом карату
			{
				ISLOGGER_I("Worker is started with port " + QString::number(Port));
				Result = Application.exec() == EXIT_SUCCESS ? true : false;
			}
			else //Не удалось запустить сервер
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
		ISLOGGER_E("Invalid arguments");
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
