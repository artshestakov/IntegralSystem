#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGHelper
{
public:
	static bool CheckExistColumn(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //��������� ������������� ���� � ������� ����
	static bool CommentTable(PMetaTable *MetaTable, QString &ErrorString); //�������� �������� � �������
	static bool CommentField(const QString &TableName, const QString &FieldName, const QString &Description, QString &ErrorString); //�������� �������� � ����
};
//-----------------------------------------------------------------------------
