#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISConsole.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    ISCaratApplication CaratApplication(argc, argv);
    bool Result = CaratApplication.Init();
    if (Result) //Приложение успешно инициализировалось
    {
        Result = CaratApplication.Start() == EXIT_SUCCESS;
    }
    else //При инициализации возникли проблемы - остановим логгер, чтобы ошибка записалась в лог
    {
        ISLogger::Instance().Shutdown();
    }
#if defined(DEBUG) && defined(WIN32) //???
    ISConsole::Pause();
#endif
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
