#pragma once
#ifndef _ISDATABASE_H_INCLUDED
#define _ISDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isserver_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISSERVER_EXPORT ISDatabase
{
public:
	static ISDatabase& Instance();

	QString GetErrorString() const; //�������� �������� ��������� ������

	QSqlDatabase GetDB(const QString &ConnectionName); //�������� ������ �� ��������� �� �� ����� ����������
	ISConnectOptionDB GetOption(const QString &ConnectionName); //�������� ��������� ����������� � ��

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //��������� ������������� ���� ������
	QVariant GetValue(const QString &TableName, const QString &FieldName, int ObjectID) const; //�������� �������� ���� �� ������� �� �������������� ������

	bool Connect(const QString &ConnectionName, const ISConnectOptionDB &ConnectOptionDB); //����������� � ���� ������
	bool Connect(const QString &ConnectionName, const QString &Host, unsigned short Port, const QString &Database, const QString &Login, const QString &Password); //����������� � ���� ������
	void Disconnect(const QString &ConnectionName); //����������� �� �� �� ����� ����������
	void DisconnectAll(); //���������� �� ���� ����������� ��

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

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
