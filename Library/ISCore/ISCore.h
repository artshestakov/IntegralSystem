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
	static bool CalendarCloseEvent(int CalendarID); //��������� ������� ���������

	//������ ���������
	static void PhoneNumberPrepare(QString &PhoneNumber); //����������� ����� �������� (������� ��� ����� ����)
	static void PhoneNumberFormat(QString &PhoneNumber); //������������� ����� ��������
};
//-----------------------------------------------------------------------------
