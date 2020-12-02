#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISDatabaseHelper
{
public:
	static void CheckValue(QVariant &Value); //�������� ��������
	static QVariant GetObjectIDToList(PMetaTable *MetaTable, PMetaField *MetaField, int ObjectID); //�������� ������������� ������� �� �����������
	static int GetCountRows(const QString &TableName);
};
//-----------------------------------------------------------------------------
