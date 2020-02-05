#include "StdAfx.h"
#include "ISCaratApplication.h"
#include "EXDefines.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISCommandLine.h"
#include "ISSystem.h"
#include "ISSettingsDatabase.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISCrashDumper.h"
#include "ISQuery.h"
#include "ISExceptionBase.h"
#include "ISQueryExceptionSyntax.h"
#include "ISApplicationRunning.h"
#include "ISLicense.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISCaratApplication CaratService(argc, argv);

	ISConfig::GetInstance().Initialize();
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CARAT);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);

	ISApplicationRunning ApplicationRunning(CARAT_UID);
	if (!ApplicationRunning.TryToRun()) //���� ���������� ��� ��������
	{
		ISDebug::ShowString(LANG("AlreadyStarted"));
		ISSystem::SleepSeconds(3);
		return EXIT_SUCCESS;
	}

	if (!CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).length() || !CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).length())
	{
		ISDebug::ShowString(LANG("NotLoginOrPassword"));
		return EXIT_FAILURE;
	}

	if (!CaratService.ConnectToDB()) //���� ����������� � ���� ������ �� ���������
	{
		ISCommandLine::Pause();
		return EXIT_FAILURE;
	}

	if (!ISLicense::GetInstance().Initialize()) //���� �������� �� ������������������
	{
		ISDebug::ShowWarningString(LANG("License.Failed").arg(ISLicense::GetInstance().GetErrorString()));
		ISCommandLine::Pause();
		return EXIT_SUCCESS;
	}

	try
	{
		return CaratService.exec();
	}
	catch (ISExceptionBase &e)
	{
		return EXIT_FAILURE;
	}
}
//-----------------------------------------------------------------------------
