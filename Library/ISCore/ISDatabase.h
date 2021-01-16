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

	QString GetErrorString() const; //�������� �������� ��������� ������

	QSqlDatabase GetDB(const QString &ConnectionName); //�������� ������ �� ��������� �� �� ����� ����������
	ISConnectOptionDB GetOption(const QString &ConnectionName); //�������� ��������� ����������� � ��

	bool CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist); //��������� ������������� ���� ������
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

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
