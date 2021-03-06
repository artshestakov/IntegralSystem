#include "ISQuery.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "ISDatabase.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
#include "ISRevision.h"
#include "ISConfigurations.h"
#include "RCC.h"
//-----------------------------------------------------------------------------
#include "CGConfigurator.h"
#include "CGConfiguratorCreate.h"
#include "CGConfiguratorUpdate.h"
#include "CGConfiguratorDelete.h"
#include "CGConfiguratorService.h"
#include "CGConfiguratorShow.h"
//-----------------------------------------------------------------------------
static std::string QS_DATABASE = PREPARE_QUERYN("SELECT COUNT(*) "
    "FROM pg_database "
    "WHERE datname = $1", 1);
//-----------------------------------------------------------------------------
std::vector<CGSection> Arguments;
std::string DBHost;
std::string DBName;
std::string DBLogin;
std::string DBPassword;
unsigned short DBPort = 0;
//-----------------------------------------------------------------------------
bool InitConfiguratorScheme(std::string &ErrorString); //������������� ����� �������������
bool CreateDatabase(); //�������� ������������� ��
bool CheckExistDatabase(const std::string &Database, bool &Exist); //��������� ������������� ���� ������
void InterpreterMode(bool &IsRunning); //����� ��������������
bool Execute(std::string &Argument); //��������� ��������� �������
bool Execute(std::string &Argument, std::string &SubArgument); //��������� ������� �������
CGConfiguratorBase* CreateClass(const std::string &ClassName); //������� �����
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    if (!ISLogger::Instance().Initialize(false))
    {
        ISDEBUG_L("Error init logger: " + ISLogger::Instance().GetErrorString());
        return EXIT_FAILURE;
    }
    ISLOGGER_I(__CLASS__, "Starting...");

    std::string ErrorString;
    if (!ISConsole::InstallEncoding(65001, ErrorString))
    {
        ISLOGGER_W("Console", "Not setting console encoding: %s", ErrorString.c_str());
    }

    //������ ����������������� �����
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISLOGGER_E(__CLASS__, "Error init config file: %s", + ISConfig::Instance().GetErrorString().c_str());
        return EXIT_FAILURE;
    }

    //��������� ���������� ����������������� �����
    if (!ISConfig::Instance().IsValid())
    {
        ISLOGGER_E(__CLASS__, "Config file is not valid: %s", ISConfig::Instance().GetErrorString().c_str());
        return EXIT_FAILURE;
    }

    DBHost = ISConfig::Instance().GetString("Database", "Host");
    DBPort = ISConfig::Instance().GetUShort("Database", "Port");
    DBName = ISConfig::Instance().GetString("Database", "Name");
    DBLogin = ISConfig::Instance().GetString("Database", "User");
    DBPassword = ISConfig::Instance().GetString("Database", "Password");

    if (!InitConfiguratorScheme(ErrorString))
    {
        ISLOGGER_E(__CLASS__, "Error init configurator schema: %s", ErrorString.c_str());
        return EXIT_FAILURE;
    }

    if (!CreateDatabase())
    {
        return EXIT_FAILURE;
    }

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
        ISLOGGER_E(__CLASS__, "Initialize meta data: %s", ISMetaData::Instance().GetErrorString().c_str());
        return EXIT_FAILURE;
    }

    bool Result = true;
    ISVectorString ArgumentsCMD = ISAlgorithm::ParseArgs(argc, argv);
    if (ArgumentsCMD.size() == 0)
    {
        ISLOGGER_I(__CLASS__, "Configurator [Version %d] %s %s\n"
            "Welcome to the Configurator.\n"
            "DBHost: %s\n"
            "DBName: %s\n"
            "DBPort: %d\n"
            "Configuration: %s\n"
            "Enter the \"help\" command to getting help",
            CARAT_VERSION_N, CARAT_CONFIGURATION, CARAT_PLATFORM,
            DBHost.c_str(),
            DBName.c_str(),
            DBPort,
            ConfigurationName.c_str());
        while (Result)
        {
            InterpreterMode(Result);
        }
    }
    else if (ArgumentsCMD.size() == 1)
    {
        Result = Execute(ArgumentsCMD[0]);
    }
    else if (ArgumentsCMD.size() == 2)
    {
        Result = Execute(ArgumentsCMD[0], ArgumentsCMD[1]);
    }

    //����������� �� �� � �������
    ISDatabase::Instance().Disconnect(DB_CONNECTION_DEFAULT);
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
bool InitConfiguratorScheme(std::string &ErrorString)
{
    //������ ���������� �������
    tinyxml2::XMLDocument XmlDocument;
    tinyxml2::XMLError XmlError = XmlDocument.Parse((const char *)RCC::OTHER_CONFIGURATOR_XML, RCC::OTHER_CONFIGURATOR_XML_SIZE);
    if (XmlError != tinyxml2::XMLError::XML_SUCCESS)
    {
        ErrorString = ISAlgorithm::StringF("Not parse Configurator.xml: %s", XmlDocument.ErrorStr());
        return false;
    }

    //�������� ������� ���
    tinyxml2::XMLElement *XmlElement = XmlDocument.FirstChildElement();
    if (!XmlElement)
    {
        ErrorString = "Not exist main tag";
        return false;
    }
    
    //��������� � �������
    XmlElement = XmlElement->FirstChildElement();
    if (!XmlElement)
    {
        ErrorString = "Not transition to sections";
        return false;
    }

    do //����� ������
    {
        CGSection Section{ XmlElement->Attribute("Name"), XmlElement->Attribute("ClassName"), std::vector<CGSectionItem>() };
        
        tinyxml2::XMLElement *TempElement = XmlElement->FirstChildElement();
        do  //����� �������
        {
            Section.Items.emplace_back(CGSectionItem{ TempElement->Attribute("Name"), TempElement->Attribute("Description") });
            TempElement = TempElement->NextSiblingElement();
        } while (TempElement);

        Arguments.emplace_back(Section);
        XmlElement = XmlElement->NextSiblingElement();
    } while (XmlElement);
    return true;
}
//-----------------------------------------------------------------------------
bool CreateDatabase()
{
    bool Result = ISDatabase::Instance().Connect(DB_CONNECTION_SYSTEM, DBHost, DBPort, DB_SYSTEM_NAME, DBLogin, DBPassword), Exist = true;
    if (!Result) //�� ������� ������������ � ��������� ��
    {
        ISLOGGER_E(__CLASS__, "Not connected to system database \"%s\": %s", DB_SYSTEM_NAME, ISDatabase::Instance().GetErrorString().c_str());
        return Result;
    }

    Result = CheckExistDatabase(DBName, Exist);
    if (Result) //�������� ������ ������� - �����������
    {
        if (Exist) //�� ���������� - ������������ � ������������
        {
            Result = ISDatabase::Instance().Connect(DB_CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
            if (!Result) //�� ������� ������������ � ������������ ��
            {
                ISLOGGER_E(__CLASS__, "Not connected to existed database (%s): %s", DBName.c_str(), ISDatabase::Instance().GetErrorString().c_str());
            }
        }
        else //�� �� ���������� - ������ �
        {
            //���������� � �����������, ��������� ��?
            Result = ISConsole::Question("Database \"" + DBName + "\" not exist! Create?");
            if (Result) //������� ��
            {
                ISQuery qCreateDB(ISDatabase::Instance().GetDB(DB_CONNECTION_SYSTEM),
                    ISAlgorithm::StringF("CREATE DATABASE %s WITH OWNER = %s ENCODING = 'UTF8'", DBName.c_str(), DBLogin.c_str()));
                Result = qCreateDB.Execute();
                Result ?
                    ISLOGGER_I(__CLASS__, "The \"%s\" database was created successfully. It is recommended that you run the \"update database\" command", DBName.c_str()) :
                    ISLOGGER_I(__CLASS__, "Error creating database \"%s\": %s", DBName.c_str(), qCreateDB.GetErrorString().c_str());
                if (Result) //���� �� ���� ������� - ������������ � ���
                {
                    Result = ISDatabase::Instance().Connect(DB_CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
                    if (!Result)
                    {
                        ISLOGGER_E(__CLASS__, "Not connected to database (%s): %s", DBName.c_str(), ISDatabase::Instance().GetErrorString().c_str());
                    }
                }
            }
            else //������������ ���������
            {
                ISDEBUG_W("You have refused to create a database");
            }
        }
    }
    else //�� ������� ��������� ������� ��
    {
        ISLOGGER_E(__CLASS__, "Checking exist database \"%s\": %s", DBName.c_str(), ISDatabase::Instance().GetErrorString().c_str());
    }

    //����������� �� ��������� �� � �������
    ISDatabase::Instance().Disconnect(DB_CONNECTION_SYSTEM);
    return Result;
}
//-----------------------------------------------------------------------------
bool CheckExistDatabase(const std::string &Database, bool &Exist)
{
    ISQuery qSelectDatabase(ISDatabase::Instance().GetDB(DB_CONNECTION_SYSTEM), QS_DATABASE);
    qSelectDatabase.SetShowLongQuery(false);
    qSelectDatabase.BindString(Database);
    bool Result = qSelectDatabase.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectDatabase.ReadColumn_Int(0) > 0;
    }
    return Result;
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
    std::string Command = ISConsole::GetString("\nEnter command (press Enter to exit): ");
    IsRunning = !Command.empty();
    if (IsRunning)
    {
        ISVectorString VectorString = ISAlgorithm::StringSplit(Command, ' ');
        if (VectorString.size() == 1)
        {
            Command = VectorString.front();
            ISAlgorithm::StringToLower(Command);

            if (VectorString.front() == "exit" || VectorString.front() == "quit") //�����
            {
                IsRunning = false;
                return;
            }
            Execute(VectorString.front());
        }
        else if (VectorString.size() == 2)
        {
            Execute(VectorString.front(), VectorString.at(1));
        }
        else
        {
            ISLOGGER_E(__CLASS__, "command not found");
        }
    }
}
//-----------------------------------------------------------------------------
bool Execute(std::string &Argument)
{
    ISAlgorithm::StringToLower(Argument);

    CGConfigurator Configurator(Arguments);
    bool Result = Configurator.ExistFunction(Argument);
    if (Result)
    {
        bool ReturnValue = true;
        ISTimePoint TimePoint = ISAlgorithm::GetTick();
        Result = Configurator.Invoke(Argument);
        if (Result)
        {
            ReturnValue ?
                ISLOGGER_I(__CLASS__, "Commnd \"%s\" executed with %d msec", Argument.c_str(), ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)) :
                ISLOGGER_I(__CLASS__, "Command \"%s\" executed with error: %s", Argument.c_str(), Configurator.GetErrorString().c_str());
        }
        else
        {
            ISDEBUG_E("Command \"" + Argument + "\" not executed.");
        }
        Result = ReturnValue;
    }
    else
    {
        ISDEBUG_L("Command \"" + Argument + "\" not found");
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool Execute(std::string &Argument, std::string &SubArgument)
{
    ISAlgorithm::StringToLower(Argument);
    ISAlgorithm::StringToLower(SubArgument);

    CGConfiguratorBase *CommandBase = CreateClass(Argument);
    bool Result = CommandBase->ExistFunction(SubArgument);
    if (Result)
    {
        ISTimePoint TimePoint = ISAlgorithm::GetTick();
        Result = CommandBase->Invoke(SubArgument);
        Result ?
            ISLOGGER_I(__CLASS__, "Command \"%s %s\" executed with %d msec", Argument.c_str(), SubArgument.c_str(), ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)) :
            ISLOGGER_I(__CLASS__, "Command \"%s %s\" executed with error: %s", Argument.c_str(), SubArgument.c_str(), CommandBase->GetErrorString().c_str());
    }
    else
    {
        ISDEBUG_E("Command \"" + SubArgument + "\" not found");
    }
    delete CommandBase;
    return Result;
}
//-----------------------------------------------------------------------------
CGConfiguratorBase* CreateClass(const std::string &ClassName)
{
    if (ClassName == "create")
    {
        return new CGConfiguratorCreate();
    }
    else if (ClassName == "delete")
    {
        return new CGConfiguratorDelete();
    }
    else if (ClassName == "service")
    {
        return new CGConfiguratorService();
    }
    else if (ClassName == "show")
    {
        return new CGConfiguratorShow();
    }
    else if (ClassName == "update")
    {
        return new CGConfiguratorUpdate();
    }
    return nullptr;
}
//-----------------------------------------------------------------------------
