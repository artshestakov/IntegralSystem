#pragma once
#ifndef _ISFAIL2BAN_H_INCLUDED
#define _ISFAIL2BAN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISFail2Ban : public QObject
{
public:
	static ISFail2Ban& Instance();

	void Start(); //Запуск службы
	bool IsLock(const QString &IPAddress); //Проверка блокировки адреса
	QDateTime GetUnlockDateTime(const QString &IPAddress); //Получить дату и время разблокировки адреса
	bool Add(const QString &IPAddress); //Добавить адрес в список блокировки
	void Remove(const QString &IPAddress); //Удалить адрес из списка

private:
	ISFail2Ban();
	~ISFail2Ban();
    ISFail2Ban(ISFail2Ban const &) = delete;
	ISFail2Ban(ISFail2Ban &&) = delete;
	ISFail2Ban& operator=(const ISFail2Ban &) = delete;
	ISFail2Ban& operator=(ISFail2Ban &&) = delete;

private:
	void Timeout();

private:
	ISCriticalSection CriticalSection;
	std::vector<ISFailAuthInfo> Vector;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
