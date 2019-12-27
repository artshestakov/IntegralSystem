#include "StdAfx.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISCore.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISSplashScreen.h"
#include "ISAuthorizationForm.h"
#include "ISRegisterMetaType.h"
#include "ISIntegralSystem.h"
#include "ISStartup.h"
#include "ISUpdateDownloadForm.h"
#include "ISUpdate.h"
#include "ISMessageBox.h"
#include "ISLicense.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
ISNamespace::UpdateResult CheckUpdate(); //Проверка обновлений
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISIntegralSystem IntegralApplication(argc, argv);
	if (!IntegralApplication.CheckAdminRole()) //Проверка наличия прав администратора
	{
		ISMessageBox::ShowCritical(nullptr, LOCALIZATION("Message.Error.NoAdministratorRights"));
		return EXIT_CODE_ERROR;
	}

	//Баннер
	ISSplashScreen::GetInstance().show();
	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.Initialize.System"));
	ISRegisterMetaType::RegisterMetaType();

	//Создание формы авторизации
	ISAuthorizationForm *AuthorizationForm = new ISAuthorizationForm(nullptr);
	ISSplashScreen::GetInstance().hide();
	ISSplashScreen::GetInstance().clearMessage();

	bool Result = AuthorizationForm->ExecAnimated();
	if (!Result)
	{
		return EXIT_CODE_NORMAL;
	}

	ISSplashScreen::GetInstance().show();
	QString UserLogin = AuthorizationForm->GetEnteredLogin();
	QString UserPassword = AuthorizationForm->GetEnteredPassword();
	delete AuthorizationForm;

	//Проверка обновления
	ISNamespace::UpdateResult ResultUpdate = CheckUpdate();
	if (ResultUpdate == ISNamespace::UR_ExitApplication)
	{
		return EXIT_CODE_ERROR;
	}
	else if (ResultUpdate == ISNamespace::UR_ErrorStartUpdate)
	{
		ISMessageBox::ShowWarning(nullptr, LOCALIZATION("Message.Warning.ErrorStartUpdate"));
	}

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.Initialize.CheckLicense"));
	if (ISLicense::GetInstance().Initialize())
	{
		ISObjects::GetInstance().Initialize();
		ISObjects::GetInstance().GetInterface()->RegisterMetaTypes();
	}
	else
	{
		ISSplashScreen::GetInstance().close();
		ISMessageBox::ShowWarning(nullptr, LOCALIZATION("License.Failed").arg(ISLicense::GetInstance().GetErrorString()));
		return EXIT_CODE_NORMAL;
	}

	int Startup = ISStartup::Startup(UserLogin, UserPassword);
	if (Startup != 0) //Если при запуске произошла ошибка
	{
		ISCore::ExitApplication();
		return Startup;
	}

	ISSplashScreen::GetInstance().ResetPixmap();
	return IntegralApplication.exec();
}
//-----------------------------------------------------------------------------
ISNamespace::UpdateResult CheckUpdate() //Проверка обновлений
{
	int FileID = 0;
	QString FileName;
	QString Version;

	ISSplashScreen::GetInstance().SetMessage(LOCALIZATION("Banner.Initialize.CheckUpdate"));
	bool UpdateExist = ISUpdate::GetInstance().CheckUpdate(FileID, FileName, Version);
	if (UpdateExist) //Если обновление найдено
	{
		ISSplashScreen::GetInstance().hide();

		ISUpdateDownloadForm UpdateDownloadForm(FileID, FileName, Version);
		if (UpdateDownloadForm.Exec())
		{
			bool Started = ISUpdate::GetInstance().StartInstallUpdate(FileName);
			if (Started)
			{
				return ISNamespace::UR_ExitApplication;
			}
			else
			{
				return ISNamespace::UR_ErrorStartUpdate;
			}
		}
		else
		{
			ISMessageBox::ShowWarning(nullptr, LOCALIZATION("Message.Warning.DownloadUpdateError"));
		}

		ISSplashScreen::GetInstance().show();
	}

	return ISNamespace::UR_ContinueWork;
}
//-----------------------------------------------------------------------------
