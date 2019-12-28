#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDatabase : public QObject
{
	Q_OBJECT

public:
	ISDatabase(const ISDatabase &) = delete;
	ISDatabase(ISDatabase &&) = delete;
	ISDatabase &operator=(const ISDatabase &) = delete;
	ISDatabase &operator=(ISDatabase &&) = delete;
	~ISDatabase();

	static ISDatabase& GetInstance();

	QSqlDatabase& GetDefaultDB(); //Получить стандартную базу данных
	QSqlDatabase& GetSystemDB(); //Получить системную базу данных

	bool CheckExistDatabase(const QString &Database); //Проверить существование базы данных
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

	void DisconnectFromDefaultDB(); //Отключиться от стандартной базы данных
	void DisconnectFromSystemDB(); //Отключиться от системной базы данных

	void DisconnectFromDatabase(QSqlDatabase &SqlDatabase); //Отключиться от базы данных

protected:
    bool ConnectToDatabase(QSqlDatabase &SqlDatabase, const QString &Login, const QString &Password, const QString &Database, QString &ErrorConnection); //Подключение к базе данных

private:
	ISDatabase();

	QSqlDatabase DefaultDB;
	QSqlDatabase SystemDB;
};
//-----------------------------------------------------------------------------
