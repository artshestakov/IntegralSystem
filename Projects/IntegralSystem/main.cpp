#include "StdAfx.h"
#include "ISLocalization.h"
#include "ISGui.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
#include "ISIntegralSystem.h"
#include "ISStartup.h"
#include "ISMessageBox.h"
#include "ISLogger.h"
#include "ISQueryPool.h"
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

	//�������� ����� �����������
	ISAuthForm *AuthForm = new ISAuthForm(nullptr);
	Result = AuthForm->ExecAnimated();
	if (!Result) //����� ����������� ���� �������
	{
		return EXIT_SUCCESS;
	}

	ISSplashScreen::GetInstance().show();
	QString UserLogin = AuthForm->GetEnteredLogin();
	QString UserPassword = AuthForm->GetEnteredPassword();
	delete AuthForm;

	int Startup = ISStartup::Startup(UserLogin, UserPassword);
	if (Startup) //���� ��� ������� ��������� ������
	{
		ISGui::ExitApplication();
		ISLogger::Instance().Shutdown();
		return Startup;
	}
	else //������ ������ �������
	{
		ISSplashScreen::GetInstance().ResetPixmap();
	}
	return Applicaton.exec();
}
//-----------------------------------------------------------------------------
