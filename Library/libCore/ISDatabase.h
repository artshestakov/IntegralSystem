#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDatabase
{
public:
	ISDatabase(const ISDatabase &) = delete;
	ISDatabase(ISDatabase &&) = delete;
	ISDatabase &operator=(const ISDatabase &) = delete;
	ISDatabase &operator=(ISDatabase &&) = delete;
	~ISDatabase();

	static ISDatabase& GetInstance();

	QString GetErrorString() const; //Получить описание последней ошибки
	void SetErrorString(const QString &error_string); //Изменить описание ошибки

	QSqlDatabase& GetDefaultDB(); //Получить стандартную базу данных
	QSqlDatabase& GetSystemDB(); //Получить системную базу данных
	QSqlDatabase& GetDB(const QString &ConnectionName); //Получить ссылку на экземпляр БД по имени соединения

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //Проверить существование базы данных
	QString GetVersionPostgres(); //Получить версию PostgreSQL
	QString GetCurrentDatabaseSize(); //Получить размер базы данных
	QString GetTableSize(const QString &TableName) const; //Получить размер таблицы
	QString GetTableSizeWithIndices(const QString &TableName) const; //Получить размер таблицы
	int GetTableRowCount(const QString &TableName) const; //Получить количество строк в таблице
	QString GetStartTimeServer() const; //Получить время запуска сервера баз данных
	QString GetLoadConfigurationTime(); //Получить время загрузки конфигурации
	QString GetInetClientAddress(); //Получить адрес удаленной стороны соединения
	QString GetInetServerAddress(); //Получить адрес локальной стороны соединения
	int GetServerProcessID(); //Получить код серверного процесса
	QString GetDatabaseCollate(); //Получить LC_COLLATE базы данных
	QString GetDatabaseCType(); //Получить CType базы данных
	QString GetDatabaseDataDirectory(); //Получить путь к кластеру базы данных
	QString GetAge(const QDateTime &DateTime) const;
	bool IsUserOnline(int UserID) const;
	bool IsUserOnline(const QString &UserLogin) const;
	QVariant GetValue(const QString &TableName, const QString &FieldName, int ObjectID); //Получить значение поля из таблицы по идентификатору записи

    bool ConnectToDefaultDB(const QString &Login, const QString &Password, QString &ErrorConnection); //Подключение к стандартной базе данных
    bool ConnectToSystemDB(QString &ErrorConnection); //Подключение к системной базе данных
	bool ConnectToDatabase(QSqlDatabase &SqlDatabase, const QString &Login, const QString &Password, const QString &Database, QString &ErrorConnection); //Подключение к базе данных
	
	bool Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password); //Подключение к базе данных
	void Disconnect(const QString &ConnectionName); //Отключиться от БД по имени соединения

	void DisconnectFromDefaultDB(); //Отключиться от стандартной базы данных
	void DisconnectFromSystemDB(); //Отключиться от системной базы данных

	void DisconnectFromDatabase(QSqlDatabase &SqlDatabase); //Отключиться от базы данных

private:
	ISDatabase();

	QString ErrorString;
	QSqlDatabase DefaultDB;
	QSqlDatabase SystemDB;
	std::map<QString, QSqlDatabase> Connections;
};
//-----------------------------------------------------------------------------
