#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISSplashScreen.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISStartup
{
public:
	static bool Startup(ISSplashScreen *SplashScreen);
	static void Shutdown(ISSplashScreen *SplashScreen);

private:
	static bool IsValidUser(ISSplashScreen *SplashScreen); //Проверка валидности пользователя
	static bool CheckAccessDatabase(ISSplashScreen *SplashScreen); //Проверка доступа к базе
	static bool LoadLocalNames(ISSplashScreen *SplashScreen); //Загрузка переопределнных имен полей
};
//-----------------------------------------------------------------------------
