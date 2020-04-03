#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCore
{
public:
	static bool Startup(bool IsGui, QString &ErrorString); //������������� ����
	static void ExitApplication(); //����� �� ���������
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //�������� ������������ �������

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