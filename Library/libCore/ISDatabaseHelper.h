#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDatabaseHelper
{
public:
	static QVariant CheckValue(QVariant &Value); //�������� ��������
	static QVariant GetObjectIDToList(PMetaClassTable *MetaTable, PMetaClassField *MetaField, int ObjectID); //�������� ������������� ������� �� �����������
	static int GetCountRows(const QString &TableName, const QString &Alias);
};
//-----------------------------------------------------------------------------
