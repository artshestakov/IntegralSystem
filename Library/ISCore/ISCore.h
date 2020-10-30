#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCore
{
public:
	static QString GetObjectName(const QString &TableName, int ObjectID); //�������� ������������ �������
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //�������� ������������ �������

	//���������
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //�������� ������� � ���������
	static bool CalendarCloseEvent(int CalendarID); //��������� ������� ���������

	//������
	static bool TaskCheckExist(int TaskID); //��������� ������������� ������ �� � ������

	//�������
	static bool SetIsDeletedObject(bool IsDeleted, PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //�������/������������ ������
	static bool SetIsDeletedObjects(bool IsDeleted, PMetaTable *MetaTable, const ISVectorInt &Objects, QString &ErrorString); //�������/������������ �������
	static bool DeleteCascadeObject(PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //������� ������ ��������

	//������ ���������
	static void PhoneNumberPrepare(QString &PhoneNumber); //����������� ����� �������� (������� ��� ����� ����)
	static void PhoneNumberFormat(QString &PhoneNumber); //������������� ����� ��������
};
//-----------------------------------------------------------------------------
