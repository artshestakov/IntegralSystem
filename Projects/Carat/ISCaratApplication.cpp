#include "ISCaratApplication.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISDebug.h"
#include "ISTcpServer.h"
#include "ISConfig.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
#include "ISMetaData.h"
#include "ISResourcer.h"
#include "ISConfigurations.h"
#include "ISRevision.h"
#include "ISConsole.h"
#include "ISProperty.h"
#include "ISBlockedIP.h"
#include "ISCrashDumper.h"
#include "RCC.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
    : ErrorString(STRING_NO_ERROR),
    IsRunning(true),
    ServiceMode(false),
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
const std::string& ISCaratApplication::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::GetServiceMode() const
{
    return ServiceMode;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Init()
{
#ifdef WIN32 //Для Windows инициализируем библиотеку WSA
    WSADATA WsaData;
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
#endif

    //Инициализируем логгер
    if (!ISLogger::Instance().Initialize(true))
    {
        ISDEBUG_E("Error initialize logger: " + ISLogger::Instance().GetErrorString());
        return false;
    }

    //Установим обработчик ошибок
    ISCrashDumper::Install();

    if (!ISConsole::InstallEncoding(65001, ErrorString))
    {
        ISLOGGER_W("Console", "Not setting console encoding: %s", ErrorString.c_str());
    }

    //Читаем файл ресурсов
    if (!ISResourcer::Instance().LoadFile(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Resources.bin"))
    {
        ISLOGGER_E("ISResourcer", "Not read resource file: %s", ISResourcer::Instance().GetErrorString().c_str());
        return false;
    }

    if (!ISLocalization::Instance().Init(RCC::LOCALIZATION_CARAT_LANG, RCC::LOCALIZATION_CARAT_LANG_SIZE))
    {
        ISLOGGER_E("ISLocalization", "Not init file \"%s\": %s", RCC::LOCALIZATION_CARAT_LANG_NAME, ISLocalization::Instance().GetErrorString().c_str());
        return false;
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
    TCPServer = ISConfig::Instance().GetBool("TCPServer", "Include");
    
    //Выбираем активную конфигурацию
    std::string ConfigurationName = ISConfig::Instance().GetString("Other", "Configuration");
    if (ConfigurationName.empty()) //Имя конфигурации не указано
    {
        ISLOGGER_E("ISConfig", "Not specified configration name");
        return false;
    }

    if (!ISConfigurations::Instance().Set(ConfigurationName))
    {
        ISLOGGER_E(__CLASS__, "Not found configuration \"%s\"", ConfigurationName.c_str());
        return false;
    }

    //Инициализируем мета-данные
    if (!ISMetaData::Instance().Init(ConfigurationName, true, true))
    {
        ISLOGGER_E("ISMetaData", "Not init meta data: %s", ISMetaData::Instance().GetErrorString().c_str());
        return false;
    }

    //Получаем параметры подключения к БД и подключаемся
    std::string DBHost = ISConfig::Instance().GetString("Database", "Host"),
        DBName = ISConfig::Instance().GetString("Database", "Name"),
        DBUser = ISConfig::Instance().GetString("Database", "User"),
        DBPassword = ISConfig::Instance().GetString("Database", "Password");
    unsigned short DBPort = ISConfig::Instance().GetUShort("Database", "Port");
    if (!ISDatabase::Instance().Connect(DB_CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBUser, DBPassword))
    {
        ISLOGGER_E("ISDatabase", "Error connection: %s", ISDatabase::Instance().GetErrorString().c_str());
        return false;
    }

    //Загружаем заблокированные IP-адреса
    if (!ISBlockedIP::Instance().Init())
    {
        ISLOGGER_E("ISBlockedIP", "Error load blocked ip: %s", ISBlockedIP::Instance().GetErrorString().c_str());
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
int ISCaratApplication::Start()
{
    std::string Argument = Arguments.empty() ? std::string() : Arguments.front();
    ServiceMode = Argument.empty();
    if (ServiceMode) //Режим службы
    {
        //Проверим, не запущен ли уже сервис
        bool AlreadyRunning = false;
        if (!CheckRunning(AlreadyRunning)) //Ошибка проверки
        {
            ISLOGGER_E(__CLASS__, ErrorString.c_str());
            return EXIT_FAILURE;
        }

        if (AlreadyRunning) //Сервис уже запущен - выходим
        {
            ISLOGGER_E(__CLASS__, "Service already running");
            return EXIT_FAILURE;
        }

        //Проверяем валидность конфигурационного файла
        if (!ISConfig::Instance().IsValid())
        {
            ISLOGGER_E("ISConfig", "File is not valid: %s", ISConfig::Instance().GetErrorString().c_str());
            return EXIT_FAILURE;
        }

        ISLOGGER_I(__CLASS__, "Starting...\n"
            "Version: %d (%s %s) from %s %s\n"
            "Branch: %s (%s)\n"
            "Host name: %s\n"
            "User name: %s\n"
            "Main thread: %lu\n"
            "PID: %d",
            CARAT_VERSION_N, CARAT_CONFIGURATION, CARAT_PLATFORM, CARAT_BUILD_DATE, CARAT_BUILD_TIME,
            CARAT_BRANCH_NAME, CARAT_HASH,
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

        //Установим метку времени работы
        ISProperty::Instance().SetUptime(ISDateTime::GetCurrentUnixtime());

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

                //Отключаемся от всех БД
                ISDatabase::Instance().DisconnectAll();

                //Выгружаем WSA
#ifdef WIN32
                if (WSACleanup() != 0) 
                {
                    ISLOGGER_E(__CLASS__, "not clean WSA: %s", ISAlgorithm::GetLastErrorS().c_str());
                }
#endif
                //На всякий случай немного подождём и завершим работу логгера
                ISSleep(100);
                ISLOGGER_I(__CLASS__, "Stopped server");
                break;
            }
        }
    }
    else if (Argument == "--shutdown" || Argument == "-s") //Режим остановки службы
    {
        Shutdown();
    }
    else if (Argument == "--help" || Argument == "-h") //Вывод помощи на консоль
    {
        Help();
    }
    else if (Argument == "--version" || Argument == "-v") //Вывод версии на консоль
    {
        Version();
    }
    else if (Argument == "--conf-create" || Argument == "-cc") //Создать конфигурационный файл
    {
        ConfigCreate();
    }
    else //Аргумент неопознан
    {
        ISDEBUG_L("Invalid argument: " + Argument);
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::CheckRunning(bool &AlreadyRunning)
{
    AlreadyRunning = false;

    ISSocketAddr SocketAddress;
    int Inet = inet_pton(AF_INET, "127.0.0.1", &SocketAddress.sin_addr.s_addr);
    if (Inet != 1)
    {
        ErrorString = Inet == 0 ? "Invalid address" : ISAlgorithm::GetLastErrorS();
        return false;
    }
    SocketAddress.sin_port = htons(/*ISConfig::Instance().GetValueUShort("TcpServer", "Port")*/49999); //Задаём порт
    SocketAddress.sin_family = AF_INET;
    
    ISSocket Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }

    if (bind(Socket, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR)
    {
        //Адрес уже используется
#ifdef WIN32
        if (ISAlgorithm::GetLastErrorN() == WSAEADDRINUSE)
#else
        if (ISAlgorithm::GetLastErrorN() == EADDRINUSE)
#endif
        {
            AlreadyRunning = true;
        }
        else //Неизвестная ошибка
        {
            ErrorString = ISAlgorithm::GetLastErrorS();
            return false;
        }
    }
    return true;
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
void ISCaratApplication::Shutdown()
{
    std::ofstream File(FileShutdown);
    File.close();
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Help()
{
#ifdef WIN32
    ISDEBUG_L("Usage: Carat.exe [argument]");
#else
    ISDEBUG_L("Usage: ./Carat [argument]");
#endif
    ISDEBUG;
    ISDEBUG_L("Arguments:");
    ISDEBUG_L("  -h,\t--help\t\t\tshow this help and exit");
    ISDEBUG_L("  -v,\t--version\t\tshow version and exit");
    ISDEBUG_L("  -s,\t--shutdown\t\tshutdown service");
    ISDEBUG_L("  -cc,\t--conf-create\t\tcreate config");
    ISDEBUG;
#ifdef WIN32
    ISDEBUG_L("Example: Carat.exe (service mode)");
#else
    ISDEBUG_L("Example: ./Carat (service mode)");
#endif
    ISDEBUG_L("* No arguments needed to start in service mode");
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Version()
{
    ISDEBUG_L(ISAlgorithm::StringF("Carat (%d) %s %s from %s %s\n%s (%s)",
        CARAT_VERSION_N, CARAT_CONFIGURATION, CARAT_PLATFORM, CARAT_BUILD_DATE, CARAT_BUILD_TIME,
        CARAT_BRANCH_NAME, CARAT_HASH));
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ConfigCreate()
{
    //Функция должна быть пустой
    //Конфыигурационный файл создаётся автоматически в функции ISCaratApplication::Init()
}
//-----------------------------------------------------------------------------
