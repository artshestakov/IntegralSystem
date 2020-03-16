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

	QString GetErrorString() const; //�������� �������� ��������� ������
	void SetErrorString(const QString &error_string); //�������� �������� ������

	QSqlDatabase& GetDefaultDB(); //�������� ����������� ���� ������
	QSqlDatabase& GetSystemDB(); //�������� ��������� ���� ������
	QSqlDatabase& GetDB(const QString &ConnectionName); //�������� ������ �� ��������� �� �� ����� ����������

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //��������� ������������� ���� ������
	QString GetVersionPostgres(); //�������� ������ PostgreSQL
	QString GetCurrentDatabaseSize(); //�������� ������ ���� ������
	QString GetTableSize(const QString &TableName) const; //�������� ������ �������
	QString GetTableSizeWithIndices(const QString &TableName) const; //�������� ������ �������
	int GetTableRowCount(const QString &TableName) const; //�������� ���������� ����� � �������
	QString GetStartTimeServer() const; //�������� ����� ������� ������� ��� ������
	QString GetLoadConfigurationTime(); //�������� ����� �������� ������������
	QString GetInetClientAddress(); //�������� ����� ��������� ������� ����������
	QString GetInetServerAddress(); //�������� ����� ��������� ������� ����������
	int GetServerProcessID(); //�������� ��� ���������� ��������
	QString GetDatabaseCollate(); //�������� LC_COLLATE ���� ������
	QString GetDatabaseCType(); //�������� CType ���� ������
	QString GetDatabaseDataDirectory(); //�������� ���� � �������� ���� ������
	QString GetAge(const QDateTime &DateTime) const;
	bool IsUserOnline(int UserID) const;
	bool IsUserOnline(const QString &UserLogin) const;
	QVariant GetValue(const QString &TableName, const QString &FieldName, int ObjectID); //�������� �������� ���� �� ������� �� �������������� ������

    bool ConnectToDefaultDB(const QString &Login, const QString &Password, QString &ErrorConnection); //����������� � ����������� ���� ������
    bool ConnectToSystemDB(QString &ErrorConnection); //����������� � ��������� ���� ������
	bool ConnectToDatabase(QSqlDatabase &SqlDatabase, const QString &Login, const QString &Password, const QString &Database, QString &ErrorConnection); //����������� � ���� ������
	
	bool Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password); //����������� � ���� ������
	void Disconnect(const QString &ConnectionName); //����������� �� �� �� ����� ����������

	void DisconnectFromDefaultDB(); //����������� �� ����������� ���� ������
	void DisconnectFromSystemDB(); //����������� �� ��������� ���� ������

	void DisconnectFromDatabase(QSqlDatabase &SqlDatabase); //����������� �� ���� ������

private:
	ISDatabase();

	QString ErrorString;
	QSqlDatabase DefaultDB;
	QSqlDatabase SystemDB;
	std::map<QString, QSqlDatabase> Connections;
};
//-----------------------------------------------------------------------------
