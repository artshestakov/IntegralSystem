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
	if (Result) //”казаны какие-то аргументы
	{
		QString Argument = CoreApplication.arguments()[1];
		if (Argument == "-d" || Argument == "--debug")
		{
			new ISCaratDebugger(&CoreApplication);
		}
		else
		{
			ISLOGGER_WARNING("Invalid argument \"" + Argument + "\"");
		}
	}
	else //јргументы не указаны - стартуем в обычном режиме
	{
		Result = ISApplicationRunning(CARAT_UID).IsRunning();
		if (Result) //≈сли приложение можно запускать
		{
			Result = ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString);
			if (Result)
			{
				Result = ISQueryText::Instance().CheckAllQueries();
				if (Result)
				{
					(new ISCaratService(&CoreApplication))->StartService();
				}
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
