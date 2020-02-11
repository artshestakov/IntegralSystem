#pragma once
#ifndef _PMETACLASSFOREIGN_H_INCLUDED
#define _PMETACLASSFOREIGN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassForeign : public PMetaClass
{
public:
	PMetaClassForeign();
	~PMetaClassForeign();

	void SetFieldName(const QString &field);
	QString GetFieldName() const;

	void SetForeignClass(const QString &foreign_class);
	QString GetForeignClass() const;

	void SetForeignField(const QString &foreign_field);
	QString GetForeginField() const;

	void SetForeignViewNameField(const QString &foreign_view_name);
	QString GetForeignViewNameField() const;

	void SetOrderField(const QString &order_field);
	QString GetOrderField() const;

	void SetTableName(const QString &table_name);
	QString GetTableName() const;

	void SetSqlQuery(const QString &sql_query);
	QString GetSqlQuery() const;

private:
	QString Field; //����, �� ������� ��������������� ������� ����
	QString ForeignClass; //�� ����� ������� ���������� ������� ����
	QString ForeignField; //�� ����� ���� ���������� ������� ����
	QString ForeignViewNameField; //����� ���� (����) ���������� � ������� �� �������
	QString OrderField; //���� �� �������� ����� ����������� ����������

	QString TableName; //�������, ���������� ����, �� ������� ��������������� ������� ����
	QString SqlQuery; //������ �� ������ �� �������� �����
};
//-----------------------------------------------------------------------------
#endif
