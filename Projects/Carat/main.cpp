#include "StdAfx.h"
#include "ISCaratApplication.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISCommandLine.h"
#include "ISSystem.h"
#include "ISSettingsDatabase.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISCrashDumper.h"
#include "ISQuery.h"
#include "ISExceptionBase.h"
#include "ISQueryExceptionSyntax.h"
#include "ISApplicationRunning.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCaratApplication CaratService(argc, argv);
	ISCrashDumper::Startup();

	ISConfig::GetInstance().Initialize(CONFIG_FILE_PATH);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CARAT);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);

	ISApplicationRunning ApplicationRunning(CARAT_UID);
	if (!ApplicationRunning.TryToRun()) //Если приложение уже запущено
	{
		ISDebug::ShowString(LOCALIZATION("AlreadyStarted"));
		ISSystem::SleepSeconds(3);
		return EXIT_CODE_NORMAL;
	}

	if (!CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).length() || !CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).length())
	{
		ISDebug::ShowString(LOCALIZATION("NotLoginOrPassword"));
		return EXIT_CODE_ERROR;
	}

	if (!CaratService.ConnectToDB()) //Если подключение к базе данных не произошло
	{
		ISCommandLine::Pause();
		return EXIT_CODE_ERROR;
	}

	if (!ISLicense::GetInstance().Initialize()) //Если лицензия не инициализировалась
	{
		ISDebug::ShowWarningString(LOCALIZATION("License.Failed").arg(ISLicense::GetInstance().GetErrorString()));
		ISCommandLine::Pause();
		return EXIT_CODE_NORMAL;
	}

	try
	{
		return CaratService.exec();
	}
	catch (ISExceptionBase &e)
	{
		return EXIT_CODE_ERROR;
	}
}
//-----------------------------------------------------------------------------
