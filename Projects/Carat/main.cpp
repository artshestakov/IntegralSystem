#include "ISCaratApplication.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISProperty.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
    int ResultCode = 0;

    //Получаем аргументы запуска и удаляем первый (путь к исполняемому файлу)
	std::vector<std::string> Arguments = ISAlgorithm::ParseCommandArgs(argc, argv);
	ISAlgorithm::VectorRemoveFirst(Arguments);
    if (Arguments.empty()) //Аргуменов запуска не было, запускаемся в режиме сервера
    {
        if (CaratApplication.Run())
        {
            ISLOGGER_I("", "Started application");
			PROPERTY_SET("Uptime", QDateTime::currentDateTime());
            ResultCode = CaratApplication.exec();

			//Останавливаем служебные сервисы и завершаем программу
			ISDatabase::Instance().DisconnectAll();
			ISLOGGER_I("", "Stopped application");
			ISLogger::Instance().Shutdown();
        }
        else
        {
            CaratApplication.Shutdown();
#ifdef DEBUG //Остановимся перед выходом с ошибкой
			system("PAUSE");
#endif
        }
    }
    else //Если аргументы запуска есть
    {
        ResultCode = CaratApplication.Run(Arguments) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    return ResultCode;
}
//-----------------------------------------------------------------------------
