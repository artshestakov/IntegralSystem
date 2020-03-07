#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISStartup : public QObject
{
	Q_OBJECT

public:
	ISStartup();
	virtual ~ISStartup();

	static int Startup(const QString &UserLogin, const QString &UserPassword);

protected:
	static bool CheckAccessDatabase(const QString &Login); //Проверка доступа к базе
	static bool CheckAccessAllowed(); //Проверка разрешения доступа пользователя
	static bool CheckExistUserGroup(); //Проверка наличия привязки пользователя к группе
};
//-----------------------------------------------------------------------------
