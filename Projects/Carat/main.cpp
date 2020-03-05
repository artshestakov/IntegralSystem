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
	if (Result) //Указаны какие-то аргументы
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
	else //Аргументы не указаны - стартуем в обычном режиме
	{
		ISApplicationRunning ApplicationRunning(CARAT_UID);
		Result = ApplicationRunning.TryToRun();
		if (Result) //Если приложение уже запущено
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
			ISLOGGER_UNKNOWN("Carat already started");
		}
	}
	return CoreApplication.exec();
}
//-----------------------------------------------------------------------------
