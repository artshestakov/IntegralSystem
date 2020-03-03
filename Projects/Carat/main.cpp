#include "StdAfx.h"
#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISConfig.h"
#include "ISExceptionBase.h"
#include "ISApplicationRunning.h"
#include "ISConstants.h"
#include "ISConsole.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCaratApplication CaratService(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (!Result)
	{
		ISLOGGER_ERROR(ErrorString);
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	ISApplicationRunning ApplicationRunning(CARAT_UID);
	if (!ApplicationRunning.TryToRun()) //Если приложение уже запущено
	{
		ISLOGGER_UNKNOWN("Application already started");
		ISConsole::Pause();
		return EXIT_SUCCESS;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty() || CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISLOGGER_UNKNOWN("Not found login or password in config");
		return EXIT_FAILURE;
	}

	if (!CaratService.ConnectToDB()) //Если подключение к базе данных не произошло
	{
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	try
	{
		return CaratService.exec();
	}
	catch (ISExceptionBase &e)
	{
		return EXIT_FAILURE;
	}
}
//-----------------------------------------------------------------------------
