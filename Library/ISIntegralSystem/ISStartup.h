#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISStartup : public QObject
{
	Q_OBJECT

public:
	ISStartup();
	virtual ~ISStartup();

	static int Startup(const QString &UserLogin, const QString &UserPassword);

protected:
	static int ExitNormal(); //Завершить программу
	static int ExitError(); //Завершить программу с ошибкой
	static void SetApplicationNameQuery(); //Изменить наименование приложения после соединения с БД
	static bool CheckAlreadyConnected(); //Проверить не онлайн ли уже текущий пользователь
	static bool CheckAccessDatabase(const QString &Login); //Проверка доступа к базе
	static bool CheckAccessAllowed(); //Проверка разрешения доступа пользователя
	static bool CheckExistUserGroup(); //Проверка наличия привязки пользователя к группе
};
//-----------------------------------------------------------------------------
