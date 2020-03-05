#include "StdAfx.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
#include "ISIntegralSystem.h"
#include "ISStartup.h"
#include "ISMessageBox.h"
#include "ISLogger.h"

#include "CLDateEdit.h"
//#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	ISIntegralSystem Applicaton(argc, argv);

	QString ErrorString;
	bool Result = ISGui::Startup(ErrorString);
	if (!Result)
	{
		ISMessageBox::ShowCritical(nullptr, ErrorString);
		return EXIT_FAILURE;
	}

	QWidget Widget;
	Widget.setLayout(new QVBoxLayout());

	CLDateEdit DateEdit(&Widget);
	Widget.layout()->addWidget(&DateEdit);

	//ISDateEdit DateEdit2(&Widget);
	//Widget.layout()->addWidget(&DateEdit2);

	Widget.show();
	Applicaton.exec();

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

	int Startup = ISStartup::Startup(UserLogin, UserPassword);
	if (Startup) //Если при запуске произошла ошибка
	{
		ISGui::ExitApplication();
		ISLogger::Instance().Shutdown();
		return Startup;
	}
	else //Запуск прошёл успешно
	{
		ISSplashScreen::GetInstance().ResetPixmap();
	}

	return Applicaton.exec();
}
//-----------------------------------------------------------------------------
