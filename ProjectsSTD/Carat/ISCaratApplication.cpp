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
    if (!ISLogger::Instance().Initialize()) //�� ������� ��������������� ������
    {
        ISDEBUG_E("Error initialize logger: " + ISLogger::Instance().GetErrorString());
        return false;
    }


    if (!ISConsole::InstallEncoding(65001, ErrorString))
    {
        ISLOGGER_W("Console", "Not setting console encoding: %s", ErrorString.c_str());
    }

    if (!ISLocalization::Instance().Init(LOCALIZATION_FILE_CARAT))
    {
        ISLOGGER_E("ISLocalization", "Not init file \"%s\": %s", LOCALIZATION_FILE_CARAT, ISLocalization::Instance().GetErrorString().c_str());
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

    //������ ���� ��������
    if (!ISResourcer::Instance().LoadFile(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Resources.bin"))
    {
        ISLOGGER_E("ISResourcer", "Not read resource file: %s", ISResourcer::Instance().GetErrorString().c_str());
        return false;
    }

    //�������� ��������� ����������������� �����
    TCPServer = ISConfig::Instance().GetValueBool("TCPServer", "Include");
    
    //�������� �������� ������������
    std::string ConfigurationName = ISConfig::Instance().GetValueString("Other", "Configuration");
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
    if (!ISMetaData::Instance().Init(ConfigurationName, true, true))
    {
        ISLOGGER_E("ISMetaData", "Not init meta data: %s", ISMetaData::Instance().GetErrorString().c_str());
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
int ISCaratApplication::Start()
{
    std::string Argument = Arguments.empty() ? std::string() : Arguments.front();
    ServiceMode = Argument.empty();
    if (ServiceMode) //����� ������
    {
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

                //�� ������ ������ ������� ������� � �������� ������ �������
                ISSleep(100);
                ISLOGGER_I(__CLASS__, "Stopped server");
                break;
            }
        }
    }
    else if (Argument == "--shutdown" || Argument == "-s") //����� ��������� ������
    {
        Shutdown();
    }
    else if (Argument == "--help" || Argument == "-h") //����� ������ �� �������
    {
        Help();
    }
    else if (Argument == "--version" || Argument == "-v") //����� ������ �� �������
    {
        Version();
    }
    else if (Argument == "--conf-create" || Argument == "-cc") //������� ���������������� ����
    {
        ConfigCreate();
    }
    else //�������� ���������
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
    //������� ������ ���� ������
    //����������������� ���� �������� ������������� � ������� ISCaratApplication::Init()
}
//-----------------------------------------------------------------------------
