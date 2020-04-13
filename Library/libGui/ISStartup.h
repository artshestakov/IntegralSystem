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

	static bool Startup();
	static void Shutdown();

private:
	static bool CheckAccessDatabase(); //Проверка доступа к базе
	static bool CheckAccessAllowed(); //Проверка разрешения доступа пользователя
	static bool CheckExistUserGroup(); //Проверка наличия привязки пользователя к группе
	static bool LoadLocalNames(); //Загрузка переопределнных имен полей
};
//-----------------------------------------------------------------------------
