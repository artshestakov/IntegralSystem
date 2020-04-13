#include "StdAfx.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
#include "ISIntegralSystem.h"
#include "ISStartup.h"
#include "ISLogger.h"
#include "ISMainWindow.h"
#include "ISSettings.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISIntegralSystem Applicaton(argc, argv);

	QString ErrorString;
	bool Result = ISGui::Startup(ErrorString);
	if (!Result)
	{
		QMessageBox::critical(nullptr, "Error", ErrorString, QMessageBox::Ok, QMessageBox::Ok);
		return EXIT_FAILURE;
	}

	//Создание формы авторизации
	ISAuthForm *AuthForm = new ISAuthForm();
	Result = AuthForm->ExecAnimated();
	if (!Result) //Форма авторизации была закрыта
	{
		return EXIT_SUCCESS;
	}

	QString UserLogin = AuthForm->GetEnteredLogin();
	QString UserPassword = AuthForm->GetEnteredPassword();
	delete AuthForm;

	ISSplashScreen SplashScreen;
	SplashScreen.show();
	SplashScreen.SetMessage(LANG("Banner.StartupSystem"));

	Result = ISStartup::Startup(UserLogin, UserPassword);
	SplashScreen.hide();

	if (Result) //Запуск прошёл успешно - создаём главное окно и ставим программу на exec()
	{
		ISMainWindow MainWindow;
		if (SETTING_BOOL(CONST_UID_SETTING_VIEW_MAINWINDOWMAXIMIZE))
		{
			SETTING_BOOL(CONST_UID_SETTING_VIEW_STARTMAINWINDOWANIMATED) ? MainWindow.ShowAnimated(true) : MainWindow.showMaximized();
		}
		else
		{
			SETTING_BOOL(CONST_UID_SETTING_VIEW_STARTMAINWINDOWANIMATED) ? MainWindow.ShowAnimated() : MainWindow.show();
		}
		MainWindow.raise();
		MainWindow.activateWindow();
		Result = Applicaton.exec() == EXIT_SUCCESS;

		SplashScreen.SetMessage(LANG("Banner.ShutdownSystem"));
		SplashScreen.show();

		ISStartup::Shutdown();
		ISGui::ExitApplication();
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
