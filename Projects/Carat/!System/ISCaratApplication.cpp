#include "StdAfx.h"
#include "ISCaratApplication.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISDatabase.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_localname "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "Carat")
{
	setServiceDescription("Carat");
	setServiceFlags(QtService::CanBeSuspended);
	createApplication(argc, argv);

	Service = new ISCaratService();
}
//-----------------------------------------------------------------------------
ISCaratApplication::~ISCaratApplication()
{

}
//-----------------------------------------------------------------------------
bool ISCaratApplication::ConnectToDB() const
{
	bool Result = true;

	QString ErrorString;
	if (ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString))
	{
		QSqlQuery SqlQuery = ISDatabase::GetInstance().GetDefaultDB().exec("SET application_name = 'Carat'");
		IS_ASSERT(SqlQuery.isActive(), QString("Set change application name failed. Error: %1").arg(SqlQuery.lastError().text()));
	}
	else
	{
		Result = false;
	}

	if (Result)
	{
		ISQueryText::GetInstance().CheckAllQueries();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::start()
{
	Service->StartService();
}
//-----------------------------------------------------------------------------
void ISCaratApplication::pause()
{
	
}
//-----------------------------------------------------------------------------
void ISCaratApplication::resume()
{
	
}
//-----------------------------------------------------------------------------
void ISCaratApplication::stop()
{
	
}
//-----------------------------------------------------------------------------
