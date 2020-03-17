#include "StdAfx.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISApplicationRunning.h"
#include "ISCore.h"
#include "ISDatabase.h"
#include "ISCaratService.h"
#include "ISCaratDebugger.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
void Usage(); //Вывод инструкции по запуску
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication CoreApplication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (!Result)
	{
		ISLOGGER_ERROR(ErrorString);
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
		else if (Argument == "-h" || Argument == "--help")
		{
			Usage();
			return EXIT_SUCCESS;
		}
		else
		{
			ISLOGGER_WARNING("Invalid argument \"" + Argument + "\"");
			Usage();
			return EXIT_SUCCESS;
		}
	}
	else //Аргументы не указаны - стартуем в обычном режиме
	{
		Result = ISApplicationRunning(CARAT_UID).IsRunning();
		if (Result) //Если приложение можно запускать
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
				ISLOGGER_ERROR("Not connected to database: " + ISDatabase::Instance().GetErrorString());
			}
		}
		else
		{
			ISLOGGER_WARNING("Carat already started");
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
	ISLOGGER_UNKNOWN("Usage: Carat [argument]");
#else
	ISLOGGER_UNKNOWN("Usage: ./Carat [argument]");
#endif
	ISLOGGER_UNKNOWN(QString());
	ISLOGGER_UNKNOWN("Arguments:");
	ISLOGGER_UNKNOWN("  -d, --debug\t\tdebug mode");
	ISLOGGER_UNKNOWN("  -h, --help\t\tshow this help");
	ISLOGGER_UNKNOWN(QString());
#ifdef WIN32
	ISLOGGER_UNKNOWN("Example: Carat.exe -d (debug mode)");
	ISLOGGER_UNKNOWN("Example: Carat.exe (service mode)");
#else
	ISLOGGER_UNKNOWN("Example: ./Carat -d (debug mode)");
	ISLOGGER_UNKNOWN("Example: ./Carat (service mode)");
#endif
	ISLOGGER_UNKNOWN("* No arguments needed to start in service mode");
}
//-----------------------------------------------------------------------------
