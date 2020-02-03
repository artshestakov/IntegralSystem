#include "ISCaratCoreApplication.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISConfig.h"
#include "ISLocalization.h"
#include "ISDatabase.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
ISCaratCoreApplication::ISCaratCoreApplication(int &argc, char **argv) : QCoreApplication(argc, argv)
{
	ISConfig::GetInstance().Initialize();
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CARAT);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_OBJECTS);
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
		if (!ISLicense::GetInstance().Initialize())
		{
			ISDebug::ShowWarningString(ISLicense::GetInstance().GetErrorString());
		}
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
