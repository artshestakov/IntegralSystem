#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISMetaViewQuery : public QObject
{
	Q_OBJECT

public:
	ISMetaViewQuery(const QString &QueryName, QObject *parent = 0);
	virtual ~ISMetaViewQuery();

	QString GetQueryText(); //�������� ����� ����-�������

	void SetClassFilter(const QString &class_filter);
	void ClearClassFilter();

	void SetOrderField(const QString &FieldName); //�������� ��� �������, �� �������� ���� ����������
	void SetOrderSort(Qt::SortOrder order_sort); //�������� ��� ����������

private:
	PMetaClassTable *MetaQuery;

	QString ClassFilter;
	QString OrderField; //��� ���� �� �������� ���� ����������
	Qt::SortOrder OrderSort; //��� ����������
};
//-----------------------------------------------------------------------------
