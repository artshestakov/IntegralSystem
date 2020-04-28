#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISSplashScreen.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISStartup
{
public:
	static bool Startup(ISSplashScreen *SplashScreen);
	static void Shutdown(ISSplashScreen *SplashScreen);

private:
	static bool IsValidUser(ISSplashScreen *SplashScreen); //�������� ���������� ������������
	static bool CheckAccessDatabase(ISSplashScreen *SplashScreen); //�������� ������� � ����
	static bool LoadLocalNames(ISSplashScreen *SplashScreen); //�������� ��������������� ���� �����
};
//-----------------------------------------------------------------------------