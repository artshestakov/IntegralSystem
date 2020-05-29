#include "ISServiceController.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISServiceController::ISServiceController(const QString &service_name)
	: ErrorString(NO_ERROR_STRING),
	ServiceName(NULL),
	SCManager(NULL),
	SCService(NULL)
{
	ServiceName = (char *)malloc(service_name.size() + 1);
	strcpy(ServiceName, service_name.toStdString().c_str());
}
//-----------------------------------------------------------------------------
ISServiceController::~ISServiceController()
{
	if (ServiceName)
	{
		free(ServiceName);
	}
}
//-----------------------------------------------------------------------------
QString ISServiceController::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISServiceController::Start()
{
	if (!OpenDB(SC_MANAGER_ALL_ACCESS))
	{
		return false;
	}

	if (StartService(SCService, NULL, NULL) == FALSE) //Ошибка запуска сервиса
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED: ErrorString = "The handle does not have the SERVICE_START access right"; break;		
		case ERROR_INVALID_HANDLE: ErrorString = "The handle is invalid"; break;
		case ERROR_PATH_NOT_FOUND: ErrorString = "The service binary file could not be found"; break;
		case ERROR_SERVICE_ALREADY_RUNNING: ErrorString = "An instance of the service is already running"; break;
		case ERROR_SERVICE_DATABASE_LOCKED: ErrorString = "The database is locked"; break;
		case ERROR_SERVICE_DEPENDENCY_DELETED: ErrorString = "The service depends on a service that does not exist or has been marked for deletion"; break;
		case ERROR_SERVICE_DEPENDENCY_FAIL: ErrorString = "The service depends on another service that has failed to start"; break;
		case ERROR_SERVICE_DISABLED: ErrorString = "The service has been disabled"; break;
		case ERROR_SERVICE_LOGON_FAILED: ErrorString = "The service did not start due to a logon failure. This error occurs if the service is configured to run under an account that does not have the \"Log on as a service\" right"; break;
		case ERROR_SERVICE_MARKED_FOR_DELETE: ErrorString = "The service has been marked for deletion"; break;
		case ERROR_SERVICE_NO_THREAD: ErrorString = "A thread could not be created for the service"; break;
		case ERROR_SERVICE_REQUEST_TIMEOUT: ErrorString = "The process for the service was started, but it did not call StartServiceCtrlDispatcher, or the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function"; break;
		}
		return false;
	}
	CloseDB();
	return true;
}
//-----------------------------------------------------------------------------
bool ISServiceController::Stop()
{
	bool Result = OpenDB(SC_MANAGER_ALL_ACCESS);
	if (Result) //БД открылась
	{
		//Запрос текущего состояния сервиса
		SERVICE_STATUS_PROCESS ServiceStatusProcess = { 0 };
		DWORD BytesNeeded;
		Result = QueryServiceStatusEx(SCService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ServiceStatusProcess, sizeof(SERVICE_STATUS_PROCESS), &BytesNeeded) == TRUE;
		if (Result) //Запрос состояния прошёл успешно
		{
			if (ServiceStatusProcess.dwCurrentState != SERVICE_STOPPED) //Если сервис запущен - останавливаем его
			{
				Result = ControlService(SCService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ServiceStatusProcess) == TRUE;
				if (Result)
				{
					while (ServiceStatusProcess.dwCurrentState != SERVICE_STOPPED) //Ожидание остановки
					{
						ISSleep(10);
						if (QueryServiceStatusEx(SCService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ServiceStatusProcess, sizeof(SERVICE_STATUS_PROCESS), &BytesNeeded) == TRUE)
						{
							if (ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
							{
								break;
							}
						}
					}
				}
			}
		}
		else //Ошибка выполнения запроса состояния
		{
			switch (GetLastError())
			{
			case ERROR_INVALID_HANDLE: ErrorString = "The handle is invalid"; break;
			case ERROR_ACCESS_DENIED: ErrorString = "The handle does not have the SERVICE_QUERY_STATUS access right"; break;
			case ERROR_INSUFFICIENT_BUFFER: ErrorString = "The buffer is too small for the SERVICE_STATUS_PROCESS structure.Nothing was written to the structure"; break;
			case ERROR_INVALID_PARAMETER: ErrorString = "The cbSize member of SERVICE_STATUS_PROCESS is not valid"; break;
			case ERROR_INVALID_LEVEL: ErrorString = "The InfoLevel parameter contains an unsupported value"; break;
			case ERROR_SHUTDOWN_IN_PROGRESS: ErrorString = "The system is shutting down; this function cannot be called"; break;
			}
		}
		CloseDB();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISServiceController::IsExist()
{
	bool Exist = OpenDB(SC_MANAGER_ENUMERATE_SERVICE);
	if (!Exist) //Не удалось открыть БД - проверяем код ошибки отсутствия сервиса
	{
		Exist = GetLastError() != ERROR_SERVICE_DOES_NOT_EXIST;
	}
	CloseDB();
	return Exist;
}
//-----------------------------------------------------------------------------
bool ISServiceController::IsRunning(bool &IsRunning)
{
	if (!OpenDB(SC_MANAGER_ENUMERATE_SERVICE)) //Не удалось открыть доступ к базе сервисов
	{
		return false;
	}
	
	SERVICE_STATUS ServiceStatus = { 0 };
	if (QueryServiceStatus(SCService, &ServiceStatus) == FALSE) //Ошибка выполнения запроса
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED: ErrorString = "The requested access was denied"; break;
		case ERROR_INVALID_HANDLE: ErrorString = "Handle is invalid"; break;
		}
		return false;
	}
	IsRunning = ServiceStatus.dwCurrentState == SERVICE_RUNNING;
	CloseDB();
	return true;
}
//-----------------------------------------------------------------------------
bool ISServiceController::OpenDB(unsigned long Access)
{
	if (!strlen(ServiceName)) //Имя сервиса не указано
	{
		ErrorString = "Service name is empty";
		return false;
	}

	SCManager = OpenSCManager(NULL, NULL, Access);
	if (SCManager == NULL) //Ошибка открытия БД сервисом
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED: ErrorString = "The requested access was denied"; break;
		case ERROR_DATABASE_DOES_NOT_EXIST: ErrorString = "The specified database does not exist"; break;
		}
		return false;
	}

	SCService = OpenService(SCManager, ServiceName, Access);
	if (SCService == NULL) //Ошибка открытия сервиса
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED: ErrorString = "The handle does not have access to the service"; break;
		case ERROR_INVALID_HANDLE: ErrorString = "The specified handle is invalid"; break;
		case ERROR_INVALID_NAME: ErrorString = "The specified service name is invalid"; break;
		case ERROR_SERVICE_DOES_NOT_EXIST: ErrorString = "The specified service does not exist"; break;
		}
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISServiceController::CloseDB()
{
	if (SCService != INVALID_HANDLE_VALUE)
	{
		CloseServiceHandle(SCService);
		SCService = NULL;
	}
	if (SCManager != INVALID_HANDLE_VALUE)
	{
		CloseServiceHandle(SCManager);
		SCManager = NULL;
	}
}
//-----------------------------------------------------------------------------
