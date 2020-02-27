#include "StdAfx.h"
#include "ISCaratApplication.h"
#include "ISLogger.h"
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

	if (!ISLogger::Instance().Initialize(true, true, "Carat"))
	{
		ISLOGGER_UNKNOWN(ISLogger::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	ISApplicationRunning ApplicationRunning(CARAT_UID);
	if (!ApplicationRunning.TryToRun()) //���� ���������� ��� ��������
	{
		ISLOGGER_UNKNOWN("Application already started");
		ISSystem::SleepSeconds(3);
		return EXIT_SUCCESS;
	}

	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty() || CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISLOGGER_UNKNOWN("Not found login or password in config");
		return EXIT_FAILURE;
	}

	if (!CaratService.ConnectToDB()) //���� ����������� � ���� ������ �� ���������
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
