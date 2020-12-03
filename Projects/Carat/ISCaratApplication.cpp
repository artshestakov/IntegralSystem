#include "ISCaratApplication.h"
#include "ISConstants.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "ISVersionInfo.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISQueryPool.h"
#include "ISMetaData.h"
#include "ISConsole.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int &argc, char **argv)
	: QCoreApplication(argc, argv),
	ErrorString(NO_ERROR_STRING),
	Controller(nullptr),
	TcpServer(nullptr),
	Asterisk(nullptr)
{
    connect(this, &ISCaratApplication::Quit, this, &ISCaratApplication::quit, Qt::QueuedConnection);
}
//-----------------------------------------------------------------------------
ISCaratApplication::~ISCaratApplication()
{

}
//-----------------------------------------------------------------------------
QString ISCaratApplication::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Initialize()
{
	//��������� ������
	if (!ISLogger::Instance().Initialize())
	{
		ISDEBUG_L(ISLogger::Instance().GetErrorString());
		return false;
	}

	//��������� ��������� ��� ����������� ���������� ��� Debug-Win32
	if (!ISConsole::InstallEncoding(65001))
	{
		ISLOGGER_W("Startup", "Error changed console encoding");
	}

	//�������� ����������� ����
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CARAT))
	{
		ISLOGGER_E("ISLocalization", QString("Not init localization file \"%1\": %2").arg(LOCALIZATION_FILE_CARAT).arg(ISLocalization::Instance().GetErrorString()));
		return false;
	}

	//������ ����� ��� ��������� ������
	if (!ISSystem::CreateDir(QCoreApplication::applicationDirPath() + "/Temp", ErrorString))
	{
		ISLOGGER_E("Startup", "Not create temp dir: " + ErrorString);
		return false;
	}

	//�������������� ���������������� ����
	if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
	{
		ISLOGGER_E("ISConfig", "Not initialize: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	//��������� ���������� ����������������� �����
	if (!ISConfig::Instance().IsValid())
	{
		ISLOGGER_E("ISConfig", "File is not valid: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	//�������� �������� ������������
	ISVersionInfo::Instance().SelectConfiguration(CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION));

	//�������������� ����-������
	if (!ISMetaData::Instance().Initialize(ISVersionInfo::Instance().ConfigurationInfo.Name, false, false))
	{
		ISLOGGER_E("ISMetaData", "Not init meta data: " + ISMetaData::Instance().GetErrorString());
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Run(const QStringList &Arguments)
{
	QString Argument = Arguments.front();
	if (Argument == "--help" || Argument == "-h")
	{
        return Help();
	}
	else if (Argument == "--version" || Argument == "-v")
	{
        return Version();
	}
	else if (Argument == "--shutdown" || Argument == "-s")
	{
        return SendShutdown();
	}
    else if (Argument == "--conf-create" || Argument == "-cc")
	{
        return ConfigCreate();
	}
    else if (Argument == "--conf-reset" || Argument == "-cr")
	{
        return ConfigReset();
	}
	else
	{
		ISDEBUG_L("Argument \"" + Argument + "\" not support");
        (void)Help();
	}
    return false;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Run()
{
    ISLOGGER(QString("Starting service...\n"
		"Version: %1 (%2 %3)\n"
		"Branch: %4 (%5)\n"
		"Host name: %6\n"
		"Domain name: %7\n"
		"OS: %8\n"
		"Main ThreadID: %9\n"
		"PID: %10\n"
		"Configuration: %11").
		arg(ISVersionInfo::Instance().ToStringVersion()).
		arg(ISVersionInfo::Instance().Info.Configuration).
		arg(ISVersionInfo::Instance().Info.Platform).
		arg(ISVersionInfo::Instance().Info.Branch).
		arg(ISVersionInfo::Instance().Info.Hash).
		arg(QSysInfo::machineHostName()).
		arg(QHostInfo::localDomainName()).
		arg(QSysInfo::prettyProductName()).
		arg(CURRENT_THREAD_ID()).
		arg(GET_PID()).
		arg(CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION)));

	//������������� ���� ��������
	if (!ISQueryPool::Instance().Start(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER),
		CONFIG_INT(CONST_CONFIG_CONNECTION_PORT),
		CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN),
		CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD)))
	{
		ISLOGGER_E("ISQueryPool", ISQueryPool::Instance().GetErrorString());
		return false;
	}

	//���� ���������� ������� - ��������� ���
	if (CONFIG_BOOL(CONST_CONFIG_CONTROLLER_INCLUDE))
	{
		Controller = new ISCaratController(this);
		if (Controller->Start())
		{
			connect(Controller, &ISCaratController::Shutdown, this, &ISCaratApplication::Shutdown);
		}
		else
		{
			ISLOGGER_E("ISCaratController", "starting failed");
			return false;
		}
	}
	
	//���� TCP-������ ������� - ��������� ���
	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE))
	{
		TcpServer = new ISTcpServer(this);
		if (!TcpServer->Run())
		{
			ISLOGGER_E("ISTcpServer", "starting failed");
			return false;
		}
	}

	//���� ������ ������ � AMI ������� - ��������� ���
	if (CONFIG_BOOL(CONST_CONFIG_AMI_INCLUDE))
	{
		Asterisk = new ISAsterisk(this);
		if (!Asterisk->Start())
		{
			ISLOGGER_E("ISAsterisk", "starting failed");
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Shutdown()
{
	ISLOGGER_I(__CLASS__, "Shutdown...");

	if (CONFIG_BOOL(CONST_CONFIG_CONTROLLER_INCLUDE) && Controller)
	{
		Controller->Stop();
	}

	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE) && TcpServer)
	{
		TcpServer->Stop();
	}

	if (CONFIG_BOOL(CONST_CONFIG_AMI_INCLUDE) && Asterisk)
	{
		Asterisk->quit();
	}
	emit Quit();
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Help()
{
#ifdef WIN32
    ISDEBUG_L("Usage: Carat.exe [argument]");
#else
	ISDEBUG_L("Usage: ./Carat [argument]");
#endif
	ISDEBUG();
	ISDEBUG_L("Arguments:");
    ISDEBUG_L("  -h,\t--help\t\tshow this help and exit");
    ISDEBUG_L("  -v,\t--version\t\tshow version and exit");
    ISDEBUG_L("  -s,\t--shutdown\t\tshutdown service");
    ISDEBUG_L("  -cc,\t--conf-create\t\tcreate config file");
    ISDEBUG_L("  -cr,\t--conf-reset\t\treset config file");
	ISDEBUG();
#ifdef WIN32
	ISDEBUG_L("Example: Carat.exe (service mode)");
#else
	ISDEBUG_L("Example: ./Carat (service mode)");
#endif
	ISDEBUG_L("* No arguments needed to start in service mode");
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Version()
{
	ISDEBUG_L("Carat (" + ISVersionInfo::Instance().ToStringVersion() + ") " + ISVersionInfo::Instance().Info.Configuration + " " + ISVersionInfo::Instance().Info.Platform);
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::SendShutdown()
{
    if(!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISDEBUG_E("Not created config: " + ISConfig::Instance().GetErrorString());
        return false;
    }
    return SendCommand(CARAT_LOCAL_API_SHUTDOWN);
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::ConfigCreate()
{
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISDEBUG_E("Not created config: " + ISConfig::Instance().GetErrorString());
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::ConfigReset()
{
    //���� ���������������� ���� �� ���������� - �������
    QString PathCurrentFile = QCoreApplication::applicationDirPath() + '/' + CONFIG_TEMPLATE_SERVER + SYMBOL_POINT + EXTENSION_INI;
    if (!QFile::exists(PathCurrentFile))
    {
        ISDEBUG_E("Config file \"" + PathCurrentFile + "\" not exist, reset impossible.");
        return false;
    }

	//��������� ����
    QFileInfo FileInfo(PathCurrentFile);
    QString PathOldFile = FileInfo.path() + "/" + FileInfo.fileName() + '.' + EXTENSION_OLD;

	QFile File(PathOldFile);

	//���� ����� ������� ����� ��� ���������� - ������� �
	if (File.exists())
	{
		if (!File.remove()) //�� ���������� ������� ������ ����
		{
			ISDEBUG_E("Error delete old file: " + File.errorString());
            return false;
		}
	}

	//��������� ��� ����� ��� �������
	File.setFileName(PathCurrentFile);

	//������ ����� �������� �����
	if (!File.copy(FileInfo.path() + "/" + FileInfo.fileName() + SYMBOL_POINT + EXTENSION_OLD)) //��� �������� ����� �������� ������
	{
		ISDEBUG_L("Error save old file: " + File.errorString());
        return false;
	}

	//������� ������� ����
	if (!File.remove()) //�� ���������� ������� ���
	{
		ISDEBUG_L("Error remove old file: " + File.errorString());
        return false;
	}

	if (!ISConfig::Instance().ReInitialize(CONFIG_TEMPLATE_SERVER))
	{
		ISDEBUG_L("Error init new file: " + ISConfig::Instance().GetErrorString());
        return false;
	}
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::SendCommand(const QByteArray &ByteArray)
{
	QTcpSocket TcpSocket;

	//������������ � ������
	ISDEBUG_L("Connecting...");
	TcpSocket.connectToHost(QHostAddress::LocalHost, CONFIG_INT(CONST_CONFIG_CONTROLLER_PORT));
	if (!TcpSocket.waitForConnected(1000))
	{
		ISDEBUG_L("Error: " + TcpSocket.errorString());
        return false;
	}

	//�������� ������
	ISDEBUG_L("Send: " + ByteArray);
    if (TcpSocket.write(ByteArray) == -1) //�� ������� ������� ������
    {
		ISDEBUG_L("Error sending: " + TcpSocket.errorString());
        return false;
    }

    if (!TcpSocket.flush())
    {
		ISDEBUG_L("Error flushing: " + TcpSocket.errorString());
        return false;
    }

	//��� ������
	ISDEBUG_L("Wait answer...");
	while (true)
	{
		ISSleep(1);
		PROCESS_EVENTS();
		if (TcpSocket.bytesAvailable() > 0) //��������� ������ - ������� � ������� � ������� �� �������
		{
			QString Answer = TcpSocket.readAll();
			ISAlgorithm::RemoveLastSymbolLoop(Answer, '\n');
			ISDEBUG_L("Answer: " + Answer);
			break;
		}
	}
    return true;
}
//-----------------------------------------------------------------------------
