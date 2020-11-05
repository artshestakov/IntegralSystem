#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISSplashScreen.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISStartup
{
public:
	static bool Startup(ISSplashScreen *SplashScreen);
	static bool StartupOld(ISSplashScreen *SplashScreen);
	static bool StartupNew(ISSplashScreen *SplashScreen);
	static void Shutdown(ISSplashScreen *SplashScreen);

private:
	static bool CheckAccessDatabase(ISSplashScreen *SplashScreen); //Проверка доступа к базе
};
//-----------------------------------------------------------------------------
