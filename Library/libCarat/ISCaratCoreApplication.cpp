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
		Result = ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString);
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCaratCoreApplication::Started()
{
	//Использовать именно такую конструкцию, иначе QProcess::readyReadAllStandartOutput почему-то не срабатывает
	qDebug() << CARAT_CORE_START_FLAG;
}
//-----------------------------------------------------------------------------
