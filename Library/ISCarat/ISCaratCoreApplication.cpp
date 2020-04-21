#include "ISCaratCoreApplication.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConstants.h"
#include "ISCore.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISCaratCoreApplication::ISCaratCoreApplication(int &argc, char **argv)
	: QCoreApplication(argc, argv),
	ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISCaratCoreApplication::~ISCaratCoreApplication()
{

}
//-----------------------------------------------------------------------------
QString ISCaratCoreApplication::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratCoreApplication::Invoke()
{
	bool Result = ISCore::Startup(false, ErrorString);
	if (Result)
	{
		Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
			CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
			CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD));
		if (!Result)
		{
			ErrorString = ISDatabase::Instance().GetErrorString();
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCaratCoreApplication::Started()
{
	ISLOGGER_L(CARAT_CORE_START_FLAG);
}
//-----------------------------------------------------------------------------
