#pragma once
#ifndef _ISDATABASE_H_INCLUDED
#define _ISDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
#include <libpq-fe.h>
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISDatabase
{
public:
	static ISDatabase& Instance();

	QString GetErrorString() const; //Получить описание последней ошибки

	//Получить ссылку на экземпляр БД по имени соединения
	QSqlDatabase GetDB(const QString &ConnectionName);
	PGconn* GetDBLibPQ(const QString &ConnectionName);
	
	//Получить параметры подключения к БД
	ISConnectOptionDB GetOption(const QString &ConnectionName);
	ISConnectOptionDB GetOptionLibPQ(const QString &ConnectionName);

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //Проверить существование базы данных

	bool ConnectLibPQ(const QString &ConnectionName, const QString &Host, unsigned short Port, const QString &Database, const QString &Login, const QString &Password); //Подключение к БД через LibPQ
	void DisconnectLibPQ(const QString &ConnectionName); //Отключение от БД через LibPQ
	void DisconnectAllLibPQ(); //Отключение от всех экземпляров БД через LibPQ

	bool Connect(const QString &ConnectionName, const ISConnectOptionDB &ConnectOptionDB); //Подключение к базе данных
	bool Connect(const QString &ConnectionName, const QString &Host, unsigned short Port, const QString &Database, const QString &Login, const QString &Password); //Подключение к базе данных
	void Disconnect(const QString &ConnectionName); //Отключиться от БД по имени соединения
	void DisconnectAll(); //Отключение от всех экземпляров БД

private:
	ISDatabase();
	~ISDatabase();
	ISDatabase(const ISDatabase&) = delete;
	ISDatabase(ISDatabase&&) = delete;
	ISDatabase& operator=(const ISDatabase&) = delete;
	ISDatabase& operator=(ISDatabase&&) = delete;

private:
	QString ErrorString;
	std::map<QString, ISConnectOptionDB> ConnectOptions;
	std::map<QString, PGconn*> ConnectionsLibPQ;

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
