#pragma once
#ifndef _ISPROTOCOL_H_INCLUDED
#define _ISPROTOCOL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISProtocol
{
public:
	static void CreateObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������� ������
	static void CreateCopyObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������� ����� ������
	static void EditObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //�������������� ������

	static void Insert(const QString &TypeUID, const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &Information = QString());
};
//-----------------------------------------------------------------------------
#endif
