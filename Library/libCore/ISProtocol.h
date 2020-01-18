#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISProtocol
{
public:
	static void EnterApplication(); //���� � ���������
	static void ExitApplication(); //����� �� ���������

	static void OpenSubSystem(const QString &TableName, const QString &LocalListName); //�������� ����������

	static void CreateObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������� ������
	static void CreateCopyObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������� ����� ������
	static void EditObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������������� ������
	static void ShowObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������� ������
	
	static void DeleteObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID); //�������� ������
	static void RecoveryObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID); //�������������� ������
	static void DeleteCascadeObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID); //��������� �������� ������

	static int Insert(bool Thread, const QString &TypeUID, const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &Information = QString());
};
//-----------------------------------------------------------------------------
