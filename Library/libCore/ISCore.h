#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCore
{
public:
	static bool Startup(QString &ErrorString); //������������� ����

	static bool DeleteOrRecoveryObject(ISNamespace::DeleteRecoveryObject DeleteOrRecovery, const QString &TableName, const QString &TableAlias, int ID, const QString &LocalListName); //�������/������������ ������
	static bool DeleteCascadeObject(const QString &TableName, const QString &TableAlias, int ObjectID); //������� ������ ��������

	static void ChangeUser(); //����� ������������
	static void RestartApplication(); //���������� ���������
	static void ExitApplication(); //����� �� ���������

	static QString GetObjectName(PMetaClassTable *MetaTable, int ObjectID); //�������� ������������ �������
	static QString GetVersionInFileName(const QString &FileName); //�������� ������ �� ������������ ����� ��������� IntegralSystem

	static void ExecuteStartCommand(); //��������� ������� ����� �������� ���������
	static void ExecuteExitComamnd(); //���������� ������� ����� �������

	//�������
	static void AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID); //�������� ������ � �������
	static void ClearHistory(); //�������� �������

	//���������
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //�������� ������� � ���������
	static bool CalendarCloseEvent(int CalendarID); //��������� ������� ���������

	//EMail
	static void EMailSend(const QString &Server, int Port, const ISUuid &ConnectionUID, const QString &SenderLogin, const QString &SenderPassword, const QString &SenderName, const QString &RecipientLogin, const QString &Subject, const QString &Message);
	static void EMailSend(const QString &RecipientLogin, const QString &Subject, const QString &Message);

	//���
	static void SMSSend(const QString &Login, const QString &Password, const QString &Phone, const QString &Message, const QString &Charset);
	static void SMSSend(const QString &Phone, const QString &Message);

	//������
	static ISUuid TaskGetStatusUID(int TaskID); //�������� ������ ������ �� � ��������������
	static void TaskSetStatus(int TaskID, const ISUuid &StatusUID, const QVariant &Resolution = QVariant()); //������� ������ ������
	static bool TaskCheckExist(int TaskID); //��������� ������������� ������ �� � ������
	static bool TaskIsDuplicate(int TaskOriginalID, int TaskDuplicateID); //��������� �������� �� ������ ����������
	static void TaskInsertDuplicate(int TaskOriginalID, int TaskDuplicateID); //�������� ������ ��� ��������
	static void TaskInsertHistory(int TaskID, int UserID, const ISUuid &HistoryUID, const QString &Information); //�������� �������� � ������� ������
	static void TaskInsertHistory(int TaskID, const ISUuid &HistoryUID, const QString &Information); //�������� �������� � ������� ������
	static bool TaskIsAttachedObject(int TaskID, const QString &TableName, int ObjectID); //��������� �� ���������� �� ������ ��� � ������
	static bool TaskAttachObject(int TaskID, const QString &TableName, int ObjectID); //���������� ������ � ������
	static bool TaskDetachObject(int TaskID, const QString &TableName, int ObjectID); //��������� ������ �� ������
	static int TaskCountOverdue(); //�������� ���������� ������������ �����
};
//-----------------------------------------------------------------------------
