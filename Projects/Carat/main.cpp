#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
#include "ISArguments.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //Формируем аргументы приложения
    ISArguments CMD(argc, argv);
    CMD.AddFlag("-h", "Show this help and exit");
    CMD.AddFlag("-v", "Show version and exit");
    CMD.AddFlag("-s", "Shutdown service");
    CMD.AddFlag("-c", "Create config");

    ISCaratApplication Carat;

    bool Result = true;
    if (CMD.IsEmpty()) //Аргументов нет - запускаемся как служба
    {
        Result = Carat.Init();
        if (Result)
        {
            Result = Carat.Start() == EXIT_SUCCESS;
        }
    }
    else //Аргументы есть - анализируем
    {
        if (CMD.IsExist("-h")) //Показать помощь
        {
            Carat.ShowHelp();
        }
        else if (CMD.IsExist("-v")) //Показать версию
        {
            Carat.ShowVersion();
        }
        else if (CMD.IsExist("-s")) //Остановка службы
        {
            Carat.Shutdown();
        }
        else if (CMD.IsExist("-c")) //Создание конфигурационного файла
        {
            Carat.ConfigCreate();
        }
    }

    //Проверим, нужно ли нам остановиться перед выходом
    if (ISAlgorithm::IsDebug() && ISAlgorithm::IsWin32() && CMD.IsEmpty())
    {
        ISConsole::Pause();
    }
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
