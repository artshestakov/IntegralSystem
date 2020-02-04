#include "StdAfx.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISBuffer.h"
#include "ISCore.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
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
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.NoAdministratorRights"));
		return EXIT_FAILURE;
	}

	//Баннер
	ISSplashScreen::GetInstance().show();
	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.System"));
	ISRegisterMetaType::RegisterMetaType();

	//Создание формы авторизации
	ISAuthForm *AuthForm = new ISAuthForm(nullptr);
	ISSplashScreen::GetInstance().hide();
	ISSplashScreen::GetInstance().clearMessage();

	bool Result = AuthForm->ExecAnimated();
	if (!Result)
	{
		return EXIT_SUCCESS;
	}

	ISSplashScreen::GetInstance().show();
	QString UserLogin = AuthForm->GetEnteredLogin();
	QString UserPassword = AuthForm->GetEnteredPassword();
	delete AuthForm;

	//Проверка обновления
	ISNamespace::UpdateResult ResultUpdate = CheckUpdate();
	if (ResultUpdate == ISNamespace::UR_ExitApplication)
	{
		return EXIT_FAILURE;
	}
	else if (ResultUpdate == ISNamespace::UR_ErrorStartUpdate)
	{
		ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.ErrorStartUpdate"));
	}

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CheckLicense"));
	if (ISLicense::GetInstance().Initialize())
	{
		ISObjects::GetInstance().Initialize();
		ISObjects::GetInstance().GetInterface()->RegisterMetaTypes();
	}
	else
	{
		ISSplashScreen::GetInstance().close();
		ISMessageBox::ShowWarning(nullptr, LANG("License.Failed").arg(ISLicense::GetInstance().GetErrorString()));
		return EXIT_SUCCESS;
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

	ISSplashScreen::GetInstance().SetMessage(LANG("Banner.Initialize.CheckUpdate"));
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
			ISMessageBox::ShowWarning(nullptr, LANG("Message.Warning.DownloadUpdateError"));
		}

		ISSplashScreen::GetInstance().show();
	}

	return ISNamespace::UR_ContinueWork;
}
//-----------------------------------------------------------------------------
