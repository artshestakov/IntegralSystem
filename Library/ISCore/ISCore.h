#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCore
{
public:
	static bool Startup(bool IsGui, const QString &ConfigTemplateName, QString &ErrorString); //������������� ����
	static void ExitApplication(); //����� �� ���������
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //�������� ������������ �������

	//�������
	static void AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID); //�������� ������ � �������
	static void ClearHistory(); //�������� �������

	//���������
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //�������� ������� � ���������
	static bool CalendarCloseEvent(int CalendarID); //��������� ������� ���������

	//������
	static bool TaskCheckExist(int TaskID); //��������� ������������� ������ �� � ������

	//�������
	static bool SetIsDeletedObject(bool IsDeleted, PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //�������/������������ ������
	static bool DeleteCascadeObject(PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //������� ������ ��������
};
//-----------------------------------------------------------------------------
