#include "ISCaratApplication.h"
#include "ISQueryPool.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISConsole.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	QObject::connect(&CaratApplication, &ISCaratApplication::Quit, &CaratApplication, &ISCaratApplication::quit, Qt::QueuedConnection);
	if (!CaratApplication.Init()) //ѕри инициализации сервера произошла ошибка - выходим
	{
		return EXIT_FAILURE;
	}

	//ѕолучаем аргументы запуска и удал€ем первый (путь к исполн€емому файлу)
	QStringList Arguments = CaratApplication.arguments();
    Arguments.removeFirst();

    int ResultCode = -1;
    if (Arguments.isEmpty()) //јргуменов запуска не было, запускаемс€ в режиме сервера
	{
        if (CaratApplication.Run())
        {
            ISLOGGER_I("", "Started application");
            ResultCode = CaratApplication.exec();
        }
		else
		{
			CaratApplication.Shutdown();
		}
	}
    else //≈сли аргументы запуска есть
    {
		CaratApplication.Run(Arguments);
		return EXIT_SUCCESS;
    }

	//ќстанавливаем служебные сервисы и завершаем программу
	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISLOGGER_I("", "Stopped application");
	ISLogger::Instance().Shutdown();

#ifdef DEBUG //¬ случае с отладочной версией ставим на паузу дл€ чтени€ ошибок и заключительного вывода
	ISConsole::Pause();
#endif
	return ResultCode;
}
//-----------------------------------------------------------------------------
