#include "ISCaratApplication.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISDebug.h"
#include "ISTcpServer.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
    : ErrorString(STRING_NO_ERROR),
    IsRunning(true),
    Arguments(ISAlgorithm::ParseArgs(argc, argv)),
    FileShutdown(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp" + PATH_SEPARATOR + "Carat.stop"),
    TCPServer(false)
{
    CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISCaratApplication::~ISCaratApplication()
{
    CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
std::string ISCaratApplication::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Init()
{
    if (!ISLogger::Instance().Initialize()) //Не удалось иницилизировать логгер
    {
        ISDEBUG_E("Error initialize logger: " + ISLogger::Instance().GetErrorString());
        return false;
    }

    if (!ISAlgorithm::ConsoleSetEncoding(65001, ErrorString))
    {
        ISLOGGER_W("Console", "Not setting console encoding: %s", ErrorString.c_str());
    }

    //Создаём папку Temp
    std::string DirTemp = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp";
    if (!ISAlgorithm::DirExist(DirTemp))
    {
        if (!ISAlgorithm::DirCreate(DirTemp, ErrorString))
        {
            ISLOGGER_E(__CLASS__, "Not created dir (%s): %s", DirTemp.c_str(), ErrorString.c_str());
            return false;
        }
    }

    //Инициализируем конфигурационный файл
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISLOGGER_E("ISConfig", "Not initialize: %s", ISConfig::Instance().GetErrorString().c_str());
        return false;
    }

    //Проверяем валидность конфигурационного файла
    if (!ISConfig::Instance().IsValid())
    {
        ISLOGGER_E("ISConfig", "File is not valid: %s", ISConfig::Instance().GetErrorString().c_str());
        return false;
    }

    //Удаляем stop-файл
    if (ISAlgorithm::FileExist(FileShutdown))
    {
        if (!ISAlgorithm::FileDelete(FileShutdown, ErrorString))
        {
            ISLOGGER_E(__CLASS__, "Not delete shutdown file (%s): %s", FileShutdown.c_str(), ErrorString.c_str());
            return false;
        }
    }

    //Получаем параметры конфигурационного файла
    TCPServer = ISConfig::Instance().GetValueBool("TCPServer", "Include");

    return true;
}
//-----------------------------------------------------------------------------
int ISCaratApplication::Start()
{
    std::string Argument = Arguments.empty() ? std::string() : Arguments.front();
    if (Argument.empty()) //Режим службы
    {
        ISLOGGER_I(__CLASS__, "Starting...\n"
            "Version: [version] ([configuration] [platform])\n"
            "Branch: [branch_name] (hash)\n"
            "Host name: %s\n"
            "User name: %s\n"
            "Main thread: %d\n"
            "PID: %d",
            ISAlgorithm::GetHostName().c_str(),
            ISAlgorithm::GetUserName().c_str(),
            CURRENT_THREAD_ID(),
            CURRENT_PID());

        //Запускаем TCP-сервер
        if (TCPServer)
        {
            if (!ISTcpServer::Instance().Start())
            {
                ISLOGGER_E(__CLASS__, "not starting TCPServer: %s", ISTcpServer::Instance().GetErrorString().c_str());
                return EXIT_FAILURE;
            }
        }

        //Запускаем поток контроля работы
        std::thread(&ISCaratApplication::ShutdownController, this).detach();
        ISLOGGER_I(__CLASS__, "Started");

        //Бесконечный цикл основного потока
        while (true)
        {
            ISSleep(1);

            //Проверяем, работать дальше или завершить работу
            CRITICAL_SECTION_LOCK(&CriticalSection);
            bool is_running = IsRunning;
            CRITICAL_SECTION_UNLOCK(&CriticalSection);
            if (!is_running)
            {
                ISLOGGER_I(__CLASS__, "Stopping server");

                //Останавливаем TCP-сервер
                if (TCPServer)
                {
                    ISTcpServer::Instance().Stop();
                }

                //На всякий случай немного подождём и завершим работу логгера
                ISSleep(500);
                ISLOGGER_I(__CLASS__, "Stopped server");
                ISLogger::Instance().Shutdown();
                break;
            }
        }
    }
    else if (Argument == "--shutdown") //Режим остановки службы
    {
        std::ofstream File(FileShutdown);
        File.close();
    }
    else //Аргумент неопознан
    {
        ISDEBUG_L("Invalid argument: " + Argument);
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ShutdownController()
{
    while (true)
    {
        ISSleep(500);
        if (ISAlgorithm::FileExist(FileShutdown)) //Если файл появился - останавливаем приложение
        {
            CRITICAL_SECTION_LOCK(&CriticalSection);
            IsRunning = false;
            CRITICAL_SECTION_UNLOCK(&CriticalSection);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
