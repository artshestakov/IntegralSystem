#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class CGHelper
{
public:
	static bool CheckExistColumn(PMetaClassTable *MetaTable, const QString &ColumnName); //��������� ������������� ���� � ������� ����
	static void CommentTable(const QString &TableName, const QString &Description); //�������� �������� � �������
	static void CommentField(const QString &TableName, const QString &FieldName, const QString &Description); //�������� �������� � ����
};
//-----------------------------------------------------------------------------
