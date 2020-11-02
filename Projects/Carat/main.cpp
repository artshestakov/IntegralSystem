#include "ISCaratApplication.h"
#include "ISQueryPool.h"
#include "ISDatabase.h"
#include "ISLogger.h"
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

    int ResultCode = -1;
    if (Arguments.isEmpty()) //Аргуменов запуска не было, запускаемся в режиме сервера
	{
        if (CaratApplication.Run())
        {
            ISLOGGER_I("", "Started application");
            ResultCode = CaratApplication.exec();
        }
	}
    else //Если аргументы запуска есть
    {
		CaratApplication.Run(Arguments);
		return EXIT_SUCCESS;
    }

	//Останавливаем служебные сервисы и завершаем программу
	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISLOGGER_I("", "Stopped application");
	ISLogger::Instance().Shutdown();
	return ResultCode;
}
//-----------------------------------------------------------------------------
