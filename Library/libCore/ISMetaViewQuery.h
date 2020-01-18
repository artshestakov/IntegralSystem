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

	QString GetQueryText(); //Получить текст мета-запроса

	void SetClassFilter(const QString &class_filter);
	void ClearClassFilter();

	void SetOrderField(const QString &FieldName); //Изменить имя столбца, по которому идет сортировка
	void SetOrderSort(Qt::SortOrder order_sort); //Изменить тип сортировки

private:
	PMetaClassTable *MetaQuery;

	QString ClassFilter;
	QString OrderField; //Имя поля по которому идет сортировка
	Qt::SortOrder OrderSort; //Тип сортировки
};
//-----------------------------------------------------------------------------
