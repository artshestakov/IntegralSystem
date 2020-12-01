#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
#include "ISQuery.h"
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
	void Protocol(int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName, const QVariant &TableLocalName, const QVariant &ObjectID, const QVariant &Information); //���������������� ��������
	void UserPasswordChange(const QVariant &UserID, const ISUuid &ChangeTypeUID); //������������ ��������� ������ ������������

private:
	bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
    QString ErrorString; //��������� �������� ������ �������
	QString DBConnectionName;
	QString DBHost;
	int DBPort;
	QString DBName;
	QString DBUser;
	QString DBPassword;
	bool IsStarted; //���� ��������� ������� �������
	ISQuery *qProtocol;
	bool IsRunning; //���� ��������� �������
	ISTcpMessage *CurrentMessage; //��������� �� ������� ���������
	bool IsStopped; //���� ��������� ������ �������

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
