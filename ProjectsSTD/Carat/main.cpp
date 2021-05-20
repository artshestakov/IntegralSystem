#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISLoggerUDP.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#define PIPE_NAME "\\\\.\\pipe\\Carat"
//-----------------------------------------------------------------------------
void Test()
{
    ISSleep(1000);

    HANDLE HandlePipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    IS_UNUSED(HandlePipe);

    while (true)
    {
        ISSleep(100);

        DWORD Readed = 0;
        char Buffer[512] = { 0 };
        if (ReadFile(HandlePipe, Buffer, 512, &Readed, NULL) == FALSE)
        {
            std::cout << ISAlgorithm::GetLastErrorS() << std::endl;
        }
        else
        {
            std::cout << Buffer << std::endl;
        }
    }
}
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    /*std::thread(&Test).detach();
    HANDLE HandlePipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 512, 512, 5000, NULL);
    if (HandlePipe != INVALID_HANDLE_VALUE)
    {
        if (ConnectNamedPipe(HandlePipe, NULL))
        {
            while (true)
            {
                ISSleep(1000);
                std::string UID = ISAlgorithm::GenerateUuid();
                DWORD dwWritten = 0;
                if (WriteFile(HandlePipe, UID.c_str(), UID.size(), &dwWritten, NULL) == FALSE)
                {
                    std::cout << ISAlgorithm::GetLastErrorS() << std::endl;
                }
            }
        }
        else
        {
            int x = 0;
            x = x;
        }
    }*/

    ISCaratApplication Carat(argc, argv);
    bool Result = Carat.Init();
    if (Result) //Приложение успешно инициализировалось - стартуем
    {
        Result = Carat.Start() == EXIT_SUCCESS;
    }
    
    //Остановим логгер
    ISLogger::Instance().Shutdown();
    ISLoggerUDP::Instance().Shutdown();

    //Проверим, нужно ли нам остановиться перед выходом
    if (ISAlgorithm::IsDebug() && ISAlgorithm::IsWin32() && Carat.GetServiceMode())
    {
        ISConsole::Pause();
    }
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
