#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISQueryModel : public QObject
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

	void SetParentFilter(int ParentObjectID, const QString &FieldName); //Изменить условие (идентификатор) фильтра для эскортной таблицы
	
	ISNamespace::PeriodType GetPeriodType() const;
	QDate GetPeriodBegin() const;
	QDate GetPeriodEnd() const;
	void SetPeriod(ISNamespace::PeriodType period_type, const QDate &period_begin, const QDate &period_end); //Установить период
	void ClearPeriod(); //Очистить период

	void SetClassFilter(const QString &class_filter); //Изменить фильтр
	QString GetClassFilter() const; //Получить фильтр
	void ClearClassFilter(); //Очистить фильтр

	void SetSearchFilter(const QString &search_filter);
	QString GetSearchFilter() const;
	void ClearSearchFilter();

	void SetOrderField(const QString &FullFieldName, const QString &FieldName, Qt::SortOrder Order); //Изменить имя столбца, по которому идет сортировка
	void SetLimit(int limit); //Изменить количество выводимых записей на странице
	void SetOffset(int offset); //Изменить страницу

private:
	QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator); //Получить текст запроса для отображаемого поля эскортной таблицы (Атрибут в схеме ForeignViewNameField)
	QString GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator); //Получить алиас для таблицы в связке LEFT JOIN

private:
	PMetaTable *MetaTable;
	ISNamespace::QueryModelType ModelType;
	QString ClassAlias;
	
	QString QuerySelectFields;
	QString QuerySelectFrom; //Отношение к таблице
	QString QuerySelectLeftJoin;
	QString QueryWhereText; //Текст условия запроса
	
	QString QueryOrderText; //Сортировка
	QString OrderField; //Имя поля по которому идет сортировка
	QString OrderFieldDefault; //Поле по умолчанию по для сортировки
	Qt::SortOrder OrderSort; //Тип сортировки
	int Limit; //Вывод записей на странице
	int Offset; //Страница

	ISNamespace::PeriodType PeriodType;
	QDate PeriodBegin;
	QDate PeriodEnd;

	QString ClassFilter;
	QString SearchFilter;

	QVariantMap Conditions;
	ISStringMap ForeignFields;
};
//-----------------------------------------------------------------------------
