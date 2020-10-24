#include "ISCaratApplication.h"
#include "ISConstants.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "ISVersion.h"
#include "ISTcpServer.h"
#include "ISCaratController.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
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
	if (!ISCore::Startup(false, "Server", ErrorString))
	{
		ISLOGGER_E("", ErrorString);
		return false;
	}

	//Загрузка локализации ядра
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CARAT))
	{
		ISLOGGER_E("ISLocalization", QString("Not init localization file \"%1\": %2").arg(LOCALIZATION_FILE_CARAT).arg(ISLocalization::Instance().GetErrorString()));
		return false;
	}

	//Проверяем наличие сервера в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISLOGGER_E("ISConfig", "server not specified");
		return false;
	}

	//Проверяем наличие порта в конфигурационном файле
	if (CONFIG_INT(CONST_CONFIG_CONNECTION_PORT) == 0)
	{
		ISLOGGER_E("ISConfig", "port not specified");
		return false;
	}

	//Проверяем наличие имени базы данных в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISLOGGER_E("ISConfig", "database name not specified");
		return false;
	}

	//Проверяем наличие логина в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty())
	{
		ISLOGGER_E("ISConfig", "login not specified");
		return false;
	}

	//Проверяем наличие пароля в конфигурационном файле
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
	if (Argument == "--version" || Argument == "-v")
	{
		Version();
	}
	else if (Argument == "--help" || Argument == "-h")
	{
		Help();
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
	ISCaratController *CaratController = new ISCaratController(this);
	if (!CaratController->Start())
	{
		return false;
	}
	
	ISTcpServer *TcpServer = new ISTcpServer(this);
	if (!TcpServer->Run())
	{
		ISLOGGER_W("ISTcpServer", "starting failed: " + TcpServer->GetErrorString());
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Version()
{
	std::cout << "Carat (" << ISVersion::Instance().ToStdString() << ") " << ISVersion::Instance().Info.Configuration.toStdString() << " " << ISVersion::Instance().Info.Platform.toStdString() << std::endl;
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
	std::cout << "  -v, --version\t\tshow version and exit" << std::endl;
	std::cout << "  -h, --help\t\tshow this help and exit" << std::endl;
	std::cout << std::endl;
#ifdef WIN32
	std::cout << "Example: Carat.exe (service mode)" << std::endl;
#else
	std::cout << "Example: ./Carat (service mode)" << std::endl;
#endif
	std::cout << "* No arguments needed to start in service mode" << std::endl;
}
//-----------------------------------------------------------------------------
