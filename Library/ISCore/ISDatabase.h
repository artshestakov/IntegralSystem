#pragma once
#ifndef _ISDATABASE_H_INCLUDED
#define _ISDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISDatabase
{
public:
	static ISDatabase& Instance();

	QString GetErrorString() const; //Получить описание последней ошибки

	QSqlDatabase GetDB(const QString &ConnectionName); //Получить ссылку на экземпляр БД по имени соединения
	ISConnectOptionDB GetOption(const QString &ConnectionName); //Получить параметры подключения к БД

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //Проверить существование базы данных
	QVariant GetValue(const QString &TableName, const QString &FieldName, int ObjectID) const; //Получить значение поля из таблицы по идентификатору записи

	bool Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password); //Подключение к базе данных
	void Disconnect(const QString &ConnectionName); //Отключиться от БД по имени соединения
	void DisconnectAll(); //Отключение от всех экземпляров БД

private:
	ISDatabase();
	~ISDatabase();
	ISDatabase(ISDatabase const &) {};
	ISDatabase& operator=(ISDatabase const&) { return *this; };

private:
	QString ErrorString;
	std::map<QString, ISConnectOptionDB> ConnectOptions;

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
