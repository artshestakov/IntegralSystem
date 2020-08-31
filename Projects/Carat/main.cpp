#include "ISLogger.h"
#include "ISConfig.h"
#include "ISApplicationRunning.h"
#include "ISCore.h"
#include "ISDatabase.h"
#include "ISCaratService.h"
#include "ISCaratDebugger.h"
#include "ISQueryText.h"
#include "ISVersion.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
void Usage(); //Вывод инструкции по запуску
bool CheckConfigValues(); //Проверка значений в конфигурационном файле
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	QCoreApplication CoreApplication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

	Result = CoreApplication.arguments().size() > 1;
	if (Result) //Указаны какие-то аргументы
	{
		QString Argument = CoreApplication.arguments()[1];
		if (Argument == "-d" || Argument == "--debug")
		{
			new ISCaratDebugger(&CoreApplication);
		}
        else if (Argument == "-v" || Argument == "--version")
        {
			std::cout << "Carat (" << ISVersion::Instance().ToStdString() << ") " << ISVersion::Instance().Info.Configuration.toStdString() << " " << ISVersion::Instance().Info.Platform.toStdString() << std::endl;
        }
		else if (Argument == "-h" || Argument == "--help")
		{
			Usage();
		}
		else
		{
			std::cout << "Invalid argument \"" << Argument.toStdString() << "\"" << std::endl;
			Usage();
		}
		ISCore::ExitApplication();
		return EXIT_SUCCESS;
	}
	else //Аргументы не указаны - стартуем в обычном режиме
	{
		Result = ISApplicationRunning(CARAT_UID).IsRunning();
		if (Result) //Если приложение можно запускать
		{
			Result = CheckConfigValues();
			if (Result) //Если все необходимые параметры заполнены - продолжаем запуск
			{
				Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
					CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
					CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD));
				if (Result)
				{
					Result = ISQueryText::Instance().CheckAllQueries();
					if (Result)
					{
						Result = (new ISCaratService(&CoreApplication))->StartService();
					}
				}
				else
				{
					ISLOGGER_E("Not connected to database: " + ISDatabase::Instance().GetErrorString());
				}
			}
		}
		else
		{
			ISLOGGER_W("Carat already started");
		}
	}

	if (Result)
	{
		return CoreApplication.exec();
	}
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
void Usage()
{
#ifdef WIN32
	std::cout << "Usage: Carat [argument]" << std::endl;
#else
	std::cout << "Usage: ./Carat [argument]" << std::endl;
#endif
	std::cout << std::endl;
	std::cout << "Arguments:" << std::endl;
	std::cout << "  -d, --debug\t\tdebug mode" << std::endl;
    std::cout << "  -v, --version\t\tshow version and exit" << std::endl;
    std::cout << "  -h, --help\t\tshow this help and exit" << std::endl;
	std::cout << std::endl;
#ifdef WIN32
	std::cout << "Example: Carat.exe -d (debug mode)" << std::endl;
	std::cout << "Example: Carat.exe (service mode)" << std::endl;
#else
	std::cout << "Example: ./Carat -d (debug mode)" << std::endl;
	std::cout << "Example: ./Carat (service mode)" << std::endl;
#endif
	std::cout << "* No arguments needed to start in service mode" << std::endl;
}
//-----------------------------------------------------------------------------
bool CheckConfigValues()
{
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISLOGGER_E("Config file: server not specified");
		return false;
	}

	if (CONFIG_INT(CONST_CONFIG_CONNECTION_PORT) == 0)
	{
		ISLOGGER_E("Config file: port not specified");
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISLOGGER_E("Config file: database name not specified");
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty())
	{
		ISLOGGER_E("Config file: login not specified");
		return false;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISLOGGER_E("Config file: password not specified");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
