#include "ISLocalization.h"
#include "ISGui.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
#include "ISStartup.h"
#include "ISLogger.h"
#include "ISMainWindow.h"
#include "ISSettings.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	QApplication Applicaton(argc, argv);
	Applicaton.setEffectEnabled(Qt::UI_General);
	Applicaton.setEffectEnabled(Qt::UI_AnimateMenu);
	Applicaton.setEffectEnabled(Qt::UI_FadeMenu);
	Applicaton.setEffectEnabled(Qt::UI_AnimateCombo);
	Applicaton.setEffectEnabled(Qt::UI_AnimateTooltip);
	Applicaton.setEffectEnabled(Qt::UI_FadeTooltip);
	Applicaton.setEffectEnabled(Qt::UI_AnimateToolBox);

	QString ErrorString;
	bool Result = ISGui::Startup(ErrorString);
	if (!Result)
	{
		QMessageBox::critical(nullptr, "Error", ErrorString, QMessageBox::Ok, QMessageBox::Ok);
		return EXIT_FAILURE;
	}

	ISAuthForm AuthForm;
	Result = AuthForm.Exec();
	if (Result) //Форма авторизации была закрыта
	{
		ISSplashScreen SplashScreen(LANG("Banner.StartupSystem"));
		SplashScreen.show();
		Result = ISStartup::Startup(&SplashScreen);
		SplashScreen.hide();

		if (Result) //Запуск прошёл успешно - создаём главное окно и ставим программу на exec()
		{
			ISMainWindow MainWindow;
			MainWindow.showMaximized();
			Result = Applicaton.exec() == EXIT_SUCCESS;

			SplashScreen.SetMessage(LANG("Banner.ShutdownSystem"));
			SplashScreen.show();
			ISStartup::Shutdown(&SplashScreen);
		}
	}
	ISGui::ExitApplication();
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
