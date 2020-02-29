#include "ISCaratCoreApplication.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISDatabase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCaratCoreApplication::ISCaratCoreApplication(int &argc, char **argv) : QCoreApplication(argc, argv)
{
	ISConfig::Instance().Initialize();
}
//-----------------------------------------------------------------------------
ISCaratCoreApplication::~ISCaratCoreApplication()
{

}
//-----------------------------------------------------------------------------
bool ISCaratCoreApplication::Invoke()
{
	QString ErrorString;
	return ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString);
}
//-----------------------------------------------------------------------------
int ISCaratCoreApplication::Exec() const
{
	return exec();
}
//-----------------------------------------------------------------------------
void ISCaratCoreApplication::Started()
{
	ISLOGGER_UNKNOWN("exec");
}
//-----------------------------------------------------------------------------
