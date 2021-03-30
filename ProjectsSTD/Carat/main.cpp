#include "ISCaratApplication.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    ISCaratApplication CaratApplication(argc, argv);
    if (CaratApplication.Init()) //Приложение успешно инициализировалось
    {
        return CaratApplication.Start();
    }
    else //При инициализации возникли проблемы - остановим логгер, чтобы ошибка записалась в лог
    {
        ISLogger::Instance().Shutdown();
    }
#ifdef DEBUG
    system("PAUSE");
#endif
    return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
