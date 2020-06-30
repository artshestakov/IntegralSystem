#include "ISLogger.h"
#include "ISConfig.h"
#include "ISApplicationRunning.h"
#include "ISCore.h"
#include "ISDatabase.h"
#include "ISCaratService.h"
#include "ISCaratDebugger.h"
#include "ISQueryText.h"
#include "ISVersion.h"
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
            std::cout << "Carat (" << ISVersion::Instance().ToString().toStdString() << ")" << std::endl;
            return EXIT_SUCCESS;
        }
		else if (Argument == "-h" || Argument == "--help")
		{
			Usage();
			return EXIT_SUCCESS;
		}
		else
		{
			ISLOGGER_W("Invalid argument \"" + Argument + "\"");
			Usage();
			return EXIT_SUCCESS;
		}
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
						(new ISCaratService(&CoreApplication))->StartService();
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
	ISLOGGER_L("Usage: Carat [argument]");
#else
	ISLOGGER_L("Usage: ./Carat [argument]");
#endif
	ISLOGGER_L(QString());
	ISLOGGER_L("Arguments:");
	ISLOGGER_L("  -d, --debug\t\tdebug mode");
    ISLOGGER_L("  -v, --version\t\tshow version and exit");
    ISLOGGER_L("  -h, --help\t\tshow this help and exit");
	ISLOGGER_L(QString());
#ifdef WIN32
	ISLOGGER_L("Example: Carat.exe -d (debug mode)");
	ISLOGGER_L("Example: Carat.exe (service mode)");
#else
	ISLOGGER_L("Example: ./Carat -d (debug mode)");
	ISLOGGER_L("Example: ./Carat (service mode)");
#endif
	ISLOGGER_L("* No arguments needed to start in service mode");
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
