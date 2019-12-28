#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaUserData : public QObject
{
	Q_OBJECT

public:
	ISMetaUserData(QObject *parent = 0);
	virtual ~ISMetaUserData();

	bool System; //Системный
	int ID; //Идентификатор пользователя
	QString Surname; //Фамилия
	QString Name; //Имя
	QString Patronymic; //Отчество
	QDate Birthday; //Дата рождения
	QString Login; //Логин
	QString Password; //Пароль
	QString IPAddress; //IP-адресс
	QString FullName; //ФИО
	bool AccessAllowed; //Доступ разрешен
	bool AccountLifetime; //Использовать срок действия учётной записи
	QDate AccountLifetimeStart; //Начало действия учётной записи
	QDate AccountLifetimeEnd; //Окончание действия учётной записи
	bool FixedInactive; //Фиксировать бездействие
	int InactiveTimeout; //Таймаут фиксирования бездействия
	
	int GroupID; //Идентификатор группы в которой состоит пользователь
	QString GroupName; //Наименование группы пользователя
	bool GroupFullAccess; //Полный доступ
};
//-----------------------------------------------------------------------------
