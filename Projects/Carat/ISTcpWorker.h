#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpWorker : public QObject
{
	Q_OBJECT

signals:
	void StartedDone();
	void StartedFailed();
	void Answer(ISTcpAnswer *);

public:
	ISTcpWorker(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password);
	virtual ~ISTcpWorker();

	bool GetStarted() const; //�������� ���� ��������� ������� �������
	bool GetRunning(); //�������� ���� ���������
	void SetMessage(ISTcpMessage *TcpMessage); //���������� ��������� �� ���������
	void Run(); //������ �������
	void Stop(); //��������� �������

private:
	void Process();
	void Finish(); //����������� � ���������� ������ �������
	QVariant CheckNullField(const QString &FieldName, const QVariantMap &VariantMap); //�������� ������� ����
	void Protocol(int UserID, const ISUuid &ActionTypeUID, const QString &TableName, const QString &TableLocalName, const QVariant &ObjectID, const QString &Information);

private:
	bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Test(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
    QString ErrorString; //��������� �������� ������ �������
	QString DBConnectionName;
	QString DBHost;
	int DBPort;
	QString DBName;
	QString DBUser;
	QString DBPassword;
	bool IsStarted; //���� ��������� ������� �������
	bool IsRunning; //���� ��������� �������
	ISTcpMessage *CurrentMessage; //��������� �� ������� ���������
	bool IsStopped; //���� ��������� ������ �������

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
