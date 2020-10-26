#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	QObject::connect(&CaratApplication, &ISCaratApplication::Quit, &CaratApplication, &ISCaratApplication::quit, Qt::QueuedConnection);
	if (!CaratApplication.Init()) //При инициализации сервера произошла ошибка - выходим
	{
		return EXIT_FAILURE;
	}

	//Получаем аргументы запуска и удаляем первый (путь к исполняемому файлу)
	QStringList Arguments = CaratApplication.arguments();
	Arguments.removeFirst();

	if (!Arguments.isEmpty()) //Если аргументы запуска есть
	{
		return CaratApplication.Run(Arguments);
	}

	int ResultCode = -1;

	//Аргуменов запуска не было, запускаемся в режиме сервера
	if (CaratApplication.Run())
	{
		ISLOGGER_I("", "Started application");
		ResultCode = CaratApplication.exec();
	}

	//Останавливаем служебные сервисы и завершаем программу
	ISDatabase::Instance().DisconnectAll();
	ISLOGGER_I("", "Stopped application");
	ISLogger::Instance().Shutdown();
	return ResultCode;
}
//-----------------------------------------------------------------------------
