#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaViewQuery
{
public:
	ISMetaViewQuery(const QString &QueryName);
	~ISMetaViewQuery();

	QString GetQueryText(); //�������� ����� ����-�������

	void SetClassFilter(const QString &class_filter);
	void ClearClassFilter();

	void SetOrderField(const QString &FieldName); //�������� ��� �������, �� �������� ���� ����������
	void SetOrderSort(Qt::SortOrder order_sort); //�������� ��� ����������

private:
	PMetaTable *MetaQuery;

	QString ClassFilter;
	QString OrderField; //��� ���� �� �������� ���� ����������
	Qt::SortOrder OrderSort; //��� ����������
};
//-----------------------------------------------------------------------------
