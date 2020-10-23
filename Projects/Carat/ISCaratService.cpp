#include "ISCaratService.h"
#include "ISLogger.h"
#include "ISCore.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
BOOL WINAPI ConsoleHandler(DWORD CtrlType)
{
	if (CtrlType == CTRL_C_EVENT)
	{
		//Тут должна быть остановка сервиса
		return TRUE;
	}
	return FALSE;
}
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "Carat")
{
	setServiceDescription("Carat is a back-end service for IntegralSystem.");
	setStartupType(QtServiceController::AutoStartup);
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{

}
//-----------------------------------------------------------------------------
void ISCaratService::start()
{
	ISLOGGER_I("Service started");
}
//-----------------------------------------------------------------------------
void ISCaratService::stop()
{
	ISLOGGER_I("Stopping services...");
	ISSleep(3000);
	ISLOGGER_I("Service stopped");
	ISLogger::Instance().Shutdown();
}
//-----------------------------------------------------------------------------
void ISCaratService::processCommand(int Code)
{
	ISLOGGER_I(QString("processCommand %1").arg(Code));
}
//-----------------------------------------------------------------------------
int ISCaratService::executeApplication()
{
	QString ErrorString;
	if (!ISCore::Startup(false, "Server", ErrorString))
	{
		return EXIT_FAILURE;
	}
	
	if (!ISDatabase::Instance().Connect("DefaultDB", "127.0.0.1", 5432, "oilsphere_db", "postgres", "adm777"))
	{
		ISLOGGER_E("Error connect to database:" + ISDatabase::Instance().GetErrorString());
		stop();
		return EXIT_FAILURE;
	}

	if (!SetConsoleCtrlHandler(&ConsoleHandler, TRUE))
	{
		ISLOGGER_W("Error install console handler: " + QString::number(GetLastError()));
	}
	return QtService<QCoreApplication>::executeApplication();
}
//-----------------------------------------------------------------------------
int ISCaratService::Error()
{
	stop();
	return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
