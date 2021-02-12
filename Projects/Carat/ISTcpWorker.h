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
	bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer); //���������� �������
	QVariant CheckNullField(const QString &FieldName, ISTcpMessage *TcpMessage); //�������� ������� ����
	void Protocol(unsigned int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName = QVariant(), const QVariant &TableLocalName = QVariant(), const QVariant &ObjectID = QVariant(), const QVariant &Information = QVariant()); //���������������� ��������
	bool UserPasswordExist(const QVariant &UserID, bool &Exist); //�������� ������� ������ � ������������
	bool UserIsSystem(const QVariant &UserID, bool &IsSystem); //�������� ������������ �� �����������
	bool GetObjectName(PMetaTable *MetaTable, unsigned int ObjectID, QString &ObjectName); //�������� ������������ �������
	PMetaTable* GetMetaTable(const QString &TableName); //�������� ��������� �� ����-�������
	bool ErrorQuery(const QString &LocalError, ISQuery &SqlQuery); //������� ������ �������

private slots:
	void RegisterOilSphere(); //����������� ������� ����������

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
	bool UserDeviceAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool UserDeviceDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordGetInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetTableQuery(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool NoteRecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool NoteRecordSet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SearchTaskText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SearchTaskID(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetCalendarEvents(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool CalendarDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetInternalLists(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetGroupRights(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GroupRightSubSystemAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GroupRightSubSystemDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GroupRightTableAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GroupRightTableDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GroupRightSpecialAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GroupRightSpecialDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetRecordValue(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordFavoriteAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool RecordFavoriteDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetFavoriteNames(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool FavoritesDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool LogGetStructure(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool LogGetContent(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool CalendarClose(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetHistoryList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool TaskCommentAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetForeignList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetServerInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OrganizationFormINN(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool StatementsQueryGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool StatementsQueryReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

	//����������
	bool OilSphere_PeriodContains(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OilSphere_GetStockList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OilSphere_StatementAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OilSphere_GetGasStation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OilSphere_GetDebtImplementation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OilSphere_GetDebtCounterparty(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool OilSphere_GetUserConsumption(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

protected:
    QString ErrorString; //��������� �������� ������ �������
	QString DBConnectionName;

private:
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

	//������ TCP-�������
	typedef bool (ISTcpWorker::*TCPFunction)(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	std::map<QString, TCPFunction> MapFunction;

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
