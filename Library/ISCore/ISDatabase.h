#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISDatabase
{
public:
	static ISDatabase& Instance();

	QString GetErrorString() const; //Получить описание последней ошибки

	QSqlDatabase GetDB(const QString &ConnectionName); //Получить ссылку на экземпляр БД по имени соединения
	ISConnectOptionDB GetOption(const QString &ConnectionName); //Получить параметры подключения к БД

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //Проверить существование базы данных
	QString GetVersionPostgres() const; //Получить версию PostgreSQL
	QString GetCurrentDatabaseSize() const; //Получить размер базы данных
	QString GetTableSize(const QString &TableName) const; //Получить размер таблицы
	QString GetTableSizeWithIndices(const QString &TableName) const; //Получить размер таблицы
	int GetTableRowCount(const QString &TableName) const; //Получить количество строк в таблице
	QString GetStartTimeServer() const; //Получить время запуска сервера баз данных
	QString GetLoadConfigurationTime() const; //Получить время загрузки конфигурации
	QString GetInetClientAddress() const; //Получить адрес удаленной стороны соединения
	QString GetInetServerAddress() const; //Получить адрес локальной стороны соединения
	int GetServerProcessID() const; //Получить код серверного процесса
	QString GetDatabaseCollate() const; //Получить LC_COLLATE базы данных
	QString GetDatabaseCType() const; //Получить CType базы данных
	QString GetDatabaseDataDirectory() const; //Получить путь к кластеру базы данных
	QString GetAge(const QDateTime &DateTime) const;
	bool IsUserOnline(int UserID) const;
	bool IsUserOnline(const QString &UserLogin) const;
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
};
//-----------------------------------------------------------------------------
