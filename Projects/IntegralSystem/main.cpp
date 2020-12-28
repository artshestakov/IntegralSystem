#include "ISLocalization.h"
#include "ISGui.h"
#include "ISSplashScreen.h"
#include "ISAuthForm.h"
#include "ISStartup.h"
#include "ISLogger.h"
#include "ISMainWindow.h"
#include "ISSettings.h"
#include "ISProperty.h"

#include <SetupAPI.h>
#include <cfgmgr32.h>
//-----------------------------------------------------------------------------
bool Method(ISVectorString &Vector)
{
	HDEVINFO DeviceInfo = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (DeviceInfo == INVALID_HANDLE_VALUE)
	{
		//������
		return false;
	}

	std::vector<std::string> v;
	for (unsigned i = 0; ; ++i)
	{
		SP_DEVINFO_DATA DeviceInfoData = { 0 };
		DeviceInfoData.cbSize = sizeof(DeviceInfoData);
		if (!SetupDiEnumDeviceInfo(DeviceInfo, i, &DeviceInfoData))
		{
			//���������� �����������
			break;
		}

		char DeviceID[MAX_DEVICE_ID_LEN];
		if (CM_Get_Device_ID(DeviceInfoData.DevInst, DeviceID, MAX_PATH, 0) != CR_SUCCESS)
		{
			//error
			continue;
		}
		v.emplace_back(std::string(DeviceID));
	}
	return true;
}
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISVectorString VectorString;
	Method(VectorString);
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
	PROPERTY_SET("PathLastDirectory", QDir::homePath());

	ISAuthForm AuthForm;
	Result = AuthForm.Exec();
	if (Result) //����� ����������� ���� ������� ��� �������� ����� � ���������
	{
		ISSplashScreen SplashScreen(LANG("Banner.StartupSystem"));
		SplashScreen.show();
		Result = ISStartup::Startup(&SplashScreen);
		SplashScreen.hide();

		if (Result) //������ ������ ������� - ������ ������� ���� � ������ ��������� �� exec()
		{
			ISMainWindow MainWindow;
			MainWindow.showMaximized();
			Result = Applicaton.exec() == EXIT_SUCCESS;

			SplashScreen.SetMessage(LANG("Banner.ShutdownSystem"));
			SplashScreen.show();
			ISStartup::Shutdown(&SplashScreen);

			//���� ���� ����� ������������ - ��������� ���������
			if (PROPERTY_GET("is_change_user").toBool())
			{
				QProcess::startDetached(QCoreApplication::applicationFilePath());
			}
		}
	}
	else //���� �� �������� ������� � ������� (��� ���-�� ���) - ��������� ������ �������
	{
		ISGui::ExitApplication();
		return EXIT_SUCCESS;
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
