#include "ISCaratApplication.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISProperty.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
    int ResultCode = 0;

    //ѕолучаем аргументы запуска и удал€ем первый (путь к исполн€емому файлу)
    QStringList Arguments = CaratApplication.arguments();
    Arguments.removeFirst();
    if (Arguments.isEmpty()) //јргуменов запуска не было, запускаемс€ в режиме сервера
    {
        if (CaratApplication.Run())
        {
            ISLOGGER_I("", "Started application");
			PROPERTY_SET("Uptime", QDateTime::currentDateTime());
            ResultCode = CaratApplication.exec();

			//ќстанавливаем служебные сервисы и завершаем программу
			ISDatabase::Instance().DisconnectAll();
			ISLOGGER_I("", "Stopped application");
			ISLogger::Instance().Shutdown();
        }
        else
        {
            CaratApplication.Shutdown();
        }
    }
    else //≈сли аргументы запуска есть
    {
        ResultCode = CaratApplication.Run(Arguments) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    return ResultCode;
}
//-----------------------------------------------------------------------------
