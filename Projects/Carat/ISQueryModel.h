#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISQueryModel : public QObject
{
	Q_OBJECT

public:
	ISQueryModel(PMetaTable *meta_table, ISNamespace::QueryModelType model_type, QObject *parent = 0);
	virtual ~ISQueryModel();

	void SetCondition(const QVariantMap &VariantMap);
	void AddCondition(const QString &Condition, const QVariant &Value);
	void ClearConditions();
	QVariantMap GetConditions();

	QString GetQueryText(); //Получить текст всего запроса

	void SetParentFilter(unsigned int ParentObjectID, const QString &FieldName); //Изменить условие (идентификатор) фильтра для эскортной таблицы

	void SetClassFilter(const QString &class_filter); //Изменить фильтр
	QString GetClassFilter() const; //Получить фильтр
	void ClearClassFilter(); //Очистить фильтр

	void SetSearchFilter(const QString &search_filter);
	QString GetSearchFilter() const;
	void ClearSearchFilter();

	QString GetSortingField() const;
	Qt::SortOrder GetSortingOrder() const;
	void SetSorting(const QString &FieldName, Qt::SortOrder Order); //Изменить имя столбца, по которому идет сортировка

	void SetLimit(int limit); //Изменить количество выводимых записей на странице
	void SetOffset(int offset); //Изменить страницу

private:
	QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator); //Получить текст запроса для отображаемого поля эскортной таблицы (Атрибут в схеме ForeignViewNameField)
	QString GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator); //Получить алиас для таблицы в связке LEFT JOIN

private:
	PMetaTable *MetaTable;
	ISNamespace::QueryModelType ModelType;
	
	QString QuerySelectFields;
	QString QuerySelectFrom; //Отношение к таблице
	QString QuerySelectLeftJoin;
	QString QueryWhereText; //Текст условия запроса
	
	QString SortingField; //Имя поля по которому идет сортировка
	Qt::SortOrder SortingOrder; //Тип сортировки
	bool SortingIsForeign; //Сортируемое поле является внешним ключом
	bool SortingIsVirtual; //Сортируемое поле является виртуальным
	int Limit; //Вывод записей на странице
	int Offset; //Страница

	QString ClassFilter;
	QString SearchFilter;

	QVariantMap WhereMap;
	ISStringMap ForeignFields;
};
//-----------------------------------------------------------------------------
