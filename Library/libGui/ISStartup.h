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
	static bool CheckAccessDatabase(const QString &Login); //Проверка доступа к базе
	static bool CheckAccessAllowed(); //Проверка разрешения доступа пользователя
	static bool CheckExistUserGroup(); //Проверка наличия привязки пользователя к группе
	static bool LoadLocalNames(); //Загрузка переопределнных имен полей
};
//-----------------------------------------------------------------------------
