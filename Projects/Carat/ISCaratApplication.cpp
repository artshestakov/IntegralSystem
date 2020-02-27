#include "ISCaratApplication.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISAssert.h"
#include "ISConfig.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_localname "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
	: QCoreApplication(argc, argv),
	Service(new ISCaratService(this))
{
	
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
void ISCaratApplication::Start()
{
	Service->StartService();
}
//-----------------------------------------------------------------------------
