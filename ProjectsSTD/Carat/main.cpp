#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    ISCaratApplication Carat(argc, argv);
    bool Result = Carat.Init();
    if (Result) //Приложение успешно инициализировалось - стартуем
    {
        Result = Carat.Start() == EXIT_SUCCESS;
    }
    //Остановим логгер
    ISLogger::Instance().Shutdown();

    //Проверим, нужно ли нам остановиться перед выходом
    if (ISAlgorithm::IsDebug() && ISAlgorithm::IsWin32() && Carat.GetServiceMode())
    {
        ISConsole::Pause();
    }
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
