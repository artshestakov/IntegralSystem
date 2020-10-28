#include "ISCaratApplication.h"
#include "ISConstants.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "ISVersionInfo.h"
#include "ISLogger.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int &argc, char **argv)
	: QCoreApplication(argc, argv),
	ErrorString(NO_ERROR_STRING)
{
	
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
bool ISCaratApplication::Init()
{
	if (!ISLogger::Instance().Initialize()) //�� ������� ��������� ������
	{
		std::cout << ISLogger::Instance().GetErrorString().toStdString() << std::endl;
		return false;
	}

#ifdef WIN32 //��������� ��������� ��� ����������� ���������� ��� Windows
	if (SetConsoleOutputCP(65001) == FALSE)
	{
		ISLOGGER_W("Startup", "Error changed console encoding");
	}
#endif

	if (!ISSystem::CreateDir(QCoreApplication::applicationDirPath() + "/Temp", ErrorString)) //�� ������� ������� ����� ��� ��������� ������
	{
		ISLOGGER_E("Startup", "Not create temp dir: " + ErrorString);
		return false;
	}

	if (!ISConfig::Instance().Initialize("Server"))
	{
		ISLOGGER_E("ISConfig", "Not initialize: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	//�������� ����������� ����
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CARAT))
	{
		ISLOGGER_E("ISLocalization", QString("Not init localization file \"%1\": %2").arg(LOCALIZATION_FILE_CARAT).arg(ISLocalization::Instance().GetErrorString()));
		return false;
	}

	//��������� ������� ������� � ���������������� �����
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISLOGGER_E("ISConfig", "server not specified");
		return false;
	}

	//��������� ������� ����� � ���������������� �����
	if (CONFIG_INT(CONST_CONFIG_CONNECTION_PORT) == 0)
	{
		ISLOGGER_E("ISConfig", "port not specified");
		return false;
	}

	//��������� ������� ����� ���� ������ � ���������������� �����
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISLOGGER_E("ISConfig", "database name not specified");
		return false;
	}

	//��������� ������� ������ � ���������������� �����
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty())
	{
		ISLOGGER_E("ISConfig", "login not specified");
		return false;
	}

	//��������� ������� ������ � ���������������� �����
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISLOGGER_E("ISConfig", "password not specified");
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
		Help();
	}
	else if (Argument == "--version" || Argument == "-v")
	{
		Version();
	}
	else if (Argument == "--shutdown" || Argument == "-s")
	{
		SendShutdown();
	}
	else
	{
		std::cout << "Invalid argument \"" << Argument.toStdString() << "\"" << std::endl;
		Help();
	}
	ISCore::ExitApplication();
	return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Run()
{
    ISLOGGER_I(__CLASS__, QString("Starting. [Version %1] %2 %3").arg(ISVersionInfo::Instance().ToString()).arg(ISVersionInfo::Instance().Info.Configuration).arg(ISVersionInfo::Instance().Info.Platform));

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
			return false;
		}
	}
	
	//���� TCP-������ ������� - ��������� ���
	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE))
	{
		TcpServer = new ISTcpServer(this);
		if (!TcpServer->Run())
		{
			ISLOGGER_W("ISTcpServer", "starting failed");
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Shutdown()
{
	ISLOGGER_I(__CLASS__, "Shutdown...");

	if (CONFIG_BOOL(CONST_CONFIG_CONTROLLER_INCLUDE))
	{
		Controller->Stop();
	}

	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE))
	{
		TcpServer->Stop();
	}
	emit Quit();
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Help()
{
#ifdef WIN32
	std::cout << "Usage: Carat [argument]" << std::endl;
#else
	std::cout << "Usage: ./Carat [argument]" << std::endl;
#endif
	std::cout << std::endl;
	std::cout << "Arguments:" << std::endl;
	std::cout << "  -h, --help\t\tshow this help and exit" << std::endl;
	std::cout << "  -v, --version\t\tshow version and exit" << std::endl;
	std::cout << "  -s, --shutdown\tshutdown service" << std::endl;
	std::cout << std::endl;
#ifdef WIN32
	std::cout << "Example: Carat.exe (service mode)" << std::endl;
#else
	std::cout << "Example: ./Carat (service mode)" << std::endl;
#endif
	std::cout << "* No arguments needed to start in service mode" << std::endl;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Version()
{
	std::cout << "Carat (" << ISVersionInfo::Instance().ToStdString() << ") " << ISVersionInfo::Instance().Info.Configuration.toStdString() << " " << ISVersionInfo::Instance().Info.Platform.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::SendShutdown()
{
	SendCommand(CARAT_LOCAL_API_SHUTDOWN);
}
//-----------------------------------------------------------------------------
void ISCaratApplication::SendCommand(const QByteArray &ByteArray)
{
	QTcpSocket TcpSocket;

	//������������ � ������
	std::cout << "Connecting..." << std::endl;
	TcpSocket.connectToHost(QHostAddress::LocalHost, CONFIG_INT(CONST_CONFIG_CONTROLLER_PORT));
	if (!TcpSocket.waitForConnected(1000))
	{
		std::cout << "Error: " << TcpSocket.errorString().toStdString() << std::endl;
		return;
	}

	//�������� ������
	std::cout << "Send: " << ByteArray.toStdString() << std::endl;
    if (TcpSocket.write(ByteArray) == -1) //�� ������� ������� ������
    {
        std::cout << "Error sending: " << TcpSocket.errorString().toStdString() << std::endl;
        return;
    }

    if (!TcpSocket.flush())
    {
        std::cout << "Error flushing: " << TcpSocket.errorString().toStdString() << std::endl;
        return;
    }

	//��� ������
    std::cout << "Wait answer..." << std::endl;
	while (true)
	{
		ISSleep(1);
		ISSystem::ProcessEvents();

		if (TcpSocket.bytesAvailable() > 0) //��������� ������ - ������� � ������� � ������� �� �������
		{
			std::cout << "Answer: " << TcpSocket.readAll().toStdString() << std::endl;
			break;
		}
	}
}
//-----------------------------------------------------------------------------