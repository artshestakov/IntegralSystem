#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGHelper
{
public:
	static bool CheckExistColumn(PMetaClassTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //��������� ������������� ���� � ������� ����
	static bool CommentTable(const QString &TableName, const QString &Description, QString &ErrorString); //�������� �������� � �������
	static bool CommentField(const QString &TableName, const QString &FieldName, const QString &Description, QString &ErrorString); //�������� �������� � ����
};
//-----------------------------------------------------------------------------
