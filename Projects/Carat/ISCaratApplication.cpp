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
#include "ISConfigurations.h"
#include "ISRevision.h"
#include "ISConsole.h"
#include "ISProperty.h"
#include "ISBlockedIP.h"
#include "ISCrashDumper.h"
#include "RCC.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication()
    : ErrorString(STRING_NO_ERROR),
    IsRunning(true),
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
bool ISCaratApplication::Init()
{
#ifdef WIN32 //��� Windows �������������� ���������� WSA
    WSADATA WsaData;
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
#endif

    //�������������� ������
    if (!ISLogger::Instance().Initialize(true))
    {
        ISDEBUG_E("Error initialize logger: " + ISLogger::Instance().GetErrorString());
        return false;
    }

    //��������� ���������� ������
    ISCrashDumper::Install();

    //��������� ���������
    if (!ISConsole::InstallEncoding(65001, ErrorString))
    {
        ISLOGGER_W("Console", "Not setting console encoding: %s", ErrorString.c_str());
    }

    //�������������� �����������
    if (!ISLocalization::Instance().Init(RCC::LOCALIZATION_CARAT_LANG, RCC::LOCALIZATION_CARAT_LANG_SIZE))
    {
        ISLOGGER_E("ISLocalization", "Not init file \"%s\": %s", RCC::LOCALIZATION_CARAT_LANG_NAME, ISLocalization::Instance().GetErrorString().c_str());
        return false;
    }

    //������ ����� Temp
    std::string DirTemp = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp";
    if (!ISAlgorithm::DirExist(DirTemp))
    {
        if (!ISAlgorithm::DirCreate(DirTemp, ErrorString))
        {
            ISLOGGER_E(__CLASS__, "Not created dir (%s): %s", DirTemp.c_str(), ErrorString.c_str());
            return false;
        }
    }

    //�������������� ���������������� ����
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISLOGGER_E("ISConfig", "Not initialize: %s", ISConfig::Instance().GetErrorString().c_str());
        return false;
    }

    //������� stop-����
    if (ISAlgorithm::FileExist(FileShutdown))
    {
        if (!ISAlgorithm::FileDelete(FileShutdown, ErrorString))
        {
            ISLOGGER_E(__CLASS__, "Not delete shutdown file (%s): %s", FileShutdown.c_str(), ErrorString.c_str());
            return false;
        }
    }

    //�������� ��������� ����������������� �����
    TCPServer = ISConfig::Instance().GetBool("TCPServer", "Include");
    
    //�������� �������� ������������
    std::string ConfigurationName = ISConfig::Instance().GetString("Other", "Configuration");
    if (ConfigurationName.empty()) //��� ������������ �� �������
    {
        ISLOGGER_E("ISConfig", "Not specified configration name");
        return false;
    }

    if (!ISConfigurations::Instance().Set(ConfigurationName))
    {
        ISLOGGER_E(__CLASS__, "Not found configuration \"%s\"", ConfigurationName.c_str());
        return false;
    }

    //�������������� ����-������
    if (!ISMetaData::Instance().Init(true, true))
    {
        ISLOGGER_E("ISMetaData", "Not init meta data: %s", ISMetaData::Instance().GetErrorString().c_str());
        return false;
    }

    //�������� ��������� ����������� � �� � ������������
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

    //��������� ��������������� IP-������
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
    //��������, �� ������� �� ��� ������
    bool AlreadyRunning = false;
    if (!CheckRunning(AlreadyRunning)) //������ ��������
    {
        ISLOGGER_E(__CLASS__, ErrorString.c_str());
        return EXIT_FAILURE;
    }

    if (AlreadyRunning) //������ ��� ������� - �������
    {
        ISLOGGER_E(__CLASS__, "Service already running");
        return EXIT_FAILURE;
    }

    //��������� ���������� ����������������� �����
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

    //��������� TCP-������
    if (TCPServer)
    {
        if (!ISTcpServer::Instance().Start())
        {
            ISLOGGER_E(__CLASS__, "not starting TCPServer: %s", ISTcpServer::Instance().GetErrorString().c_str());
            return EXIT_FAILURE;
        }
    }

    //��������� ����� �������� ������
    std::thread(&ISCaratApplication::ShutdownController, this).detach();
    ISLOGGER_I(__CLASS__, "Started");

    //��������� ����� ������� ������
    ISProperty::Instance().SetUptime(ISDateTime::GetCurrentUnixtime());

    //����������� ���� ��������� ������
    while (true)
    {
        ISSleep(1);

        //���������, �������� ������ ��� ��������� ������
        CRITICAL_SECTION_LOCK(&CriticalSection);
        bool is_running = IsRunning;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
        if (!is_running)
        {
            ISLOGGER_I(__CLASS__, "Stopping server");

            //������������� TCP-������
            if (TCPServer)
            {
                ISTcpServer::Instance().Stop();
            }

            //����������� �� ���� ��
            ISDatabase::Instance().DisconnectAll();

            //��������� WSA
#ifdef WIN32
            if (WSACleanup() != 0)
            {
                ISLOGGER_E(__CLASS__, "not clean WSA: %s", ISAlgorithm::GetLastErrorS().c_str());
            }
#endif
            //�� ������ ������ ������� ������� � �������� ������ �������
            ISSleep(100);
            ISLOGGER_I(__CLASS__, "Stopped server");
            break;
        }
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ShowVersion()
{
    ISDEBUG_L(ISAlgorithm::StringF("Carat (%d) %s %s from %s %s\n%s (%s)",
        CARAT_VERSION_N, CARAT_CONFIGURATION, CARAT_PLATFORM, CARAT_BUILD_DATE, CARAT_BUILD_TIME,
        CARAT_BRANCH_NAME, CARAT_HASH));
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Shutdown()
{
    std::ofstream(FileShutdown).close();
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ConfigCreate()
{
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISDEBUG_L(ISConfig::Instance().GetErrorString());
    }
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
    SocketAddress.sin_port = htons(49999); //����� ����
    SocketAddress.sin_family = AF_INET;
    
    ISSocket Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }

    if (bind(Socket, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR)
    {
        //����� ��� ������������
#ifdef WIN32
        if (ISAlgorithm::GetLastErrorN() == WSAEADDRINUSE)
#else
        if (ISAlgorithm::GetLastErrorN() == EADDRINUSE)
#endif
        {
            AlreadyRunning = true;
        }
        else //����������� ������
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
        ISSleep(200);
        if (ISAlgorithm::FileExist(FileShutdown)) //���� ���� �������� - ������������� ����������
        {
            CRITICAL_SECTION_LOCK(&CriticalSection);
            IsRunning = false;
            CRITICAL_SECTION_UNLOCK(&CriticalSection);
            break;
        }
    }
}
//-----------------------------------------------------------------------------
