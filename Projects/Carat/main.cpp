#include "StdAfx.h"
#include "ISCaratApplication.h"
#include "ISDebug.h"
#include "ISCommandLine.h"
#include "ISSystem.h"
#include "ISConfig.h"
#include "ISExceptionBase.h"
#include "ISApplicationRunning.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCaratApplication CaratService(argc, argv);

	ISConfig::GetInstance().Initialize();

	ISApplicationRunning ApplicationRunning(CARAT_UID);
	if (!ApplicationRunning.TryToRun()) //Если приложение уже запущено
	{
		ISDebug::ShowString("Application already started");
		ISSystem::SleepSeconds(3);
		return EXIT_SUCCESS;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty() || CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISDebug::ShowString("Not found login or password in config");
		return EXIT_FAILURE;
	}

	if (!CaratService.ConnectToDB()) //Если подключение к базе данных не произошло
	{
		ISCommandLine::Pause();
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
