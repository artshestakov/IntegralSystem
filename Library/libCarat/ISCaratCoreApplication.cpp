#include "ISCaratCoreApplication.h"
#include "ISDebug.h"
#include "ISConfig.h"
#include "ISDatabase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCaratCoreApplication::ISCaratCoreApplication(int &argc, char **argv) : QCoreApplication(argc, argv)
{
	ISConfig::GetInstance().Initialize();
}
//-----------------------------------------------------------------------------
ISCaratCoreApplication::~ISCaratCoreApplication()
{

}
//-----------------------------------------------------------------------------
bool ISCaratCoreApplication::Invoke()
{
	QString ErrorString;
	bool Result = ISDatabase::GetInstance().ConnectToDefaultDB(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), ErrorString);
	if (Result)
	{
		//???
		/*if (!ISLicense::GetInstance().Initialize())
		{
			ISDebug::ShowWarningString(ISLicense::GetInstance().GetErrorString());
		}*/
	}

	return Result;
}
//-----------------------------------------------------------------------------
int ISCaratCoreApplication::Exec() const
{
	return exec();
}
//-----------------------------------------------------------------------------
void ISCaratCoreApplication::Started()
{
	ISDebug::ShowString("exec");
}
//-----------------------------------------------------------------------------
