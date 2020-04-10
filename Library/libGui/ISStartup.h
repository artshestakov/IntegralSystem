#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISSplashScreen.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISStartup : public QObject
{
	Q_OBJECT

public:
	ISStartup();
	virtual ~ISStartup();

	static int Startup(ISSplashScreen *SplashScreen, const QString &UserLogin, const QString &UserPassword);

private:
	static bool CheckAccessDatabase(const QString &Login); //�������� ������� � ����
	static bool CheckAccessAllowed(); //�������� ���������� ������� ������������
	static bool CheckExistUserGroup(); //�������� ������� �������� ������������ � ������
	static bool LoadLocalNames(); //�������� ��������������� ���� �����
};
//-----------------------------------------------------------------------------
