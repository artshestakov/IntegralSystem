#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISDatabase
{
public:
	static ISDatabase& Instance();

	QString GetErrorString() const; //�������� �������� ��������� ������

	QSqlDatabase GetDB(const QString &ConnectionName); //�������� ������ �� ��������� �� �� ����� ����������
	ISConnectOptionDB GetOption(const QString &ConnectionName); //�������� ��������� ����������� � ��

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //��������� ������������� ���� ������
	QString GetVersionPostgres() const; //�������� ������ PostgreSQL
	QString GetCurrentDatabaseSize() const; //�������� ������ ���� ������
	QString GetTableSize(const QString &TableName) const; //�������� ������ �������
	QString GetTableSizeWithIndices(const QString &TableName) const; //�������� ������ �������
	int GetTableRowCount(const QString &TableName) const; //�������� ���������� ����� � �������
	QString GetStartTimeServer() const; //�������� ����� ������� ������� ��� ������
	QString GetLoadConfigurationTime() const; //�������� ����� �������� ������������
	QString GetInetClientAddress() const; //�������� ����� ��������� ������� ����������
	QString GetInetServerAddress() const; //�������� ����� ��������� ������� ����������
	int GetServerProcessID() const; //�������� ��� ���������� ��������
	QString GetDatabaseCollate() const; //�������� LC_COLLATE ���� ������
	QString GetDatabaseCType() const; //�������� CType ���� ������
	QString GetDatabaseDataDirectory() const; //�������� ���� � �������� ���� ������
	QString GetAge(const QDateTime &DateTime) const;
	bool IsUserOnline(int UserID) const;
	bool IsUserOnline(const QString &UserLogin) const;
	QVariant GetValue(const QString &TableName, const QString &FieldName, int ObjectID) const; //�������� �������� ���� �� ������� �� �������������� ������

	bool Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password); //����������� � ���� ������
	void Disconnect(const QString &ConnectionName); //����������� �� �� �� ����� ����������
	void DisconnectAll(); //���������� �� ���� ����������� ��

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
