#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISDatabaseHelper
{
public:
	static QVariant CheckValue(QVariant &Value); //�������� ��������
	static QVariant GetObjectIDToList(PMetaTable *MetaTable, PMetaField *MetaField, int ObjectID); //�������� ������������� ������� �� �����������
	static int GetCountRows(const QString &TableName, const QString &Alias);
};
//-----------------------------------------------------------------------------
