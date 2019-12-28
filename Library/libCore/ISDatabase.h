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

	QSqlDatabase& GetDefaultDB(); //�������� ����������� ���� ������
	QSqlDatabase& GetSystemDB(); //�������� ��������� ���� ������

	bool CheckExistDatabase(const QString &Database); //��������� ������������� ���� ������
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

	void DisconnectFromDefaultDB(); //����������� �� ����������� ���� ������
	void DisconnectFromSystemDB(); //����������� �� ��������� ���� ������

	void DisconnectFromDatabase(QSqlDatabase &SqlDatabase); //����������� �� ���� ������

protected:
    bool ConnectToDatabase(QSqlDatabase &SqlDatabase, const QString &Login, const QString &Password, const QString &Database, QString &ErrorConnection); //����������� � ���� ������

private:
	ISDatabase();

	QSqlDatabase DefaultDB;
	QSqlDatabase SystemDB;
};
//-----------------------------------------------------------------------------
