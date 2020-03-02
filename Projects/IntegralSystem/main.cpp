#include "StdAfx.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
#include "ISIntegralSystem.h"
#include "ISStartup.h"
#include "ISUpdateDownloadForm.h"
#include "ISUpdate.h"
#include "ISMessageBox.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISNamespace::UpdateResult CheckUpdate(); //Проверка обновлений
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISIntegralSystem Applicaton(argc, argv);
	
	if (!ISLogger::Instance().Initialize(true, true, "IntegralSystem"))
	{
		ISLOGGER_UNKNOWN(ISLogger::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	QString ErrorString;
	bool Result = ISGui::Startup(ErrorString);
	if (!Result)
	{
		ISMessageBox::ShowCritical(nullptr, ErrorString);
		return EXIT_FAILURE;
	}

	//Создание формы авторизации
	ISAuthForm *AuthForm = new ISAuthForm(nullptr);
	Result = AuthForm->ExecAnimated();
	if (!Result) //Форма авторизации была закрыта
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

	int Startup = ISStartup::Startup(UserLogin, UserPassword);
	if (Startup) //Если при запуске произошла ошибка
	{
		ISGui::ExitApplication();
		return Startup;
	}

	ISSplashScreen::GetInstance().ResetPixmap();
	return Applicaton.exec();
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
