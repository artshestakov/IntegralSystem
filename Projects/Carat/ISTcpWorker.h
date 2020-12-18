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
#include "PMetaClass.h"
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
	QVariant CheckNullField(const QString &FieldName, ISTcpMessage *TcpMessage); //�������� ������� ����
	void Protocol(int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName, const QVariant &TableLocalName, const QVariant &ObjectID, const QVariant &Information = QVariant()); //���������������� ��������
	bool UserPasswordExist(const QVariant &UserID, bool &Exist); //�������� ������� ������ � ������������
	bool UserIsSystem(const QVariant &UserID, bool &IsSystem); //�������� ������������ �� �����������
	QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat); //�������������� ���� � ����� � ������
	QString ConvertDateToString(const QDate &Date, const QString &DateFormat); //�������������� ���� � ������
	bool GetObjectName(PMetaTable *MetaTable, unsigned int ObjectID, QString &ObjectName); //�������� ������������ �������

private:
	bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool NoteRecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool NoteRecordSet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SearchTaskText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SearchTaskID(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

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
