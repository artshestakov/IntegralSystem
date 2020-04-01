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

	QString GetQueryText(); //Получить текст мета-запроса

	void SetClassFilter(const QString &class_filter);
	void ClearClassFilter();

	void SetOrderField(const QString &FieldName); //Изменить имя столбца, по которому идет сортировка
	void SetOrderSort(Qt::SortOrder order_sort); //Изменить тип сортировки

private:
	PMetaTable *MetaQuery;

	QString ClassFilter;
	QString OrderField; //Имя поля по которому идет сортировка
	Qt::SortOrder OrderSort; //Тип сортировки
};
//-----------------------------------------------------------------------------
