#include "StdAfx.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISApplicationRunning.h"
#include "ISCore.h"
#include "ISDatabase.h"
#include "ISCaratService.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication CoreApplication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (Result)
	{
		ISApplicationRunning ApplicationRunning(CARAT_UID);
		Result = ApplicationRunning.TryToRun();
		if (Result) //���� ���������� ��� ��������
		{
			Result = ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString);
			if (Result)
			{
				QSqlQuery SqlQuery = ISDatabase::GetInstance().GetDefaultDB().exec("SET application_name = 'Carat'");
				Result = SqlQuery.isActive();
				if (Result)
				{
					Result = ISQueryText::Instance().CheckAllQueries();
					if (Result)
					{
						(new ISCaratService(&CoreApplication))->StartService();
						CoreApplication.exec();
					}
				}
				else
				{
					ErrorString = QString("Set change application name failed. Error: %1").arg(SqlQuery.lastError().text());
				}
			}
		}
		else
		{
			ISLOGGER_UNKNOWN("Application already started");
		}
	}
	else
	{
		ISLOGGER_ERROR(ErrorString);
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
