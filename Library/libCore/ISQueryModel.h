#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISNamespace.h"
#include "ISTypes.h"
#include "ISQueryModelHelper.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryModel
{
public:
	ISQueryModel(PMetaClassTable *meta_table, ISNamespace::QueryModelType model_type);
	virtual ~ISQueryModel();

	void SetCondition(const QVariantMap &VariantMap);
	void AddCondition(const QString &Condition, const QVariant &Value);
	void ClearConditions();
	QVariantMap GetConditions();

	QString GetQueryText(); //Получить текст всего запроса

	bool GetVisibleIsDeleted() const; //Показываются ли записи помеченные на удаление
	void SetVisibleIsDeleted(bool Visible); //Показывать/скрывать удаленные записи помеченные на удаление

	void SetParentObjectIDClassFilter(int ParentObjectID); //Изменить условие (идентификатор) фильтра для эскортной таблицы
	
	ISNamespace::PeriodType GetPeriodType() const;
	ISRangeStruct GetPeriod() const;
	void SetPeriod(ISNamespace::PeriodType period_type, const ISRangeStruct &period_range); //Установить период
	void ClearPeriod(); //Очистить период

	void SetClassFilter(const QString &class_filter); //Изменить фильтр
	QString GetClassFilter() const; //Получить фильтр
	void ClearClassFilter(); //Очистить фильтр

	void SetSearchFilter(const QString &search_filter);
	QString GetSearchFilter() const;
	void ClearSearchFilter();

	void SetOrderField(const QString &FullFieldName, const QString &FieldName); //Изменить имя столбца, по которому идет сортировка
	void SetOrderSort(Qt::SortOrder order_sort); //Изменить тип сортировки
	void SetLimit(int limit); //Изменить количество выводимых записей на странице
	void SetOffset(int offset); //Изменить страницу

protected:
	void CreateQuerySelectSystemFields(); //Создание списка системных полей для запроса выборки
	void CreateQuerySelectFields(); //Создание списка полей для запроса выборки
	void CreateQuerySelectIsDeleted(); //Создание условия отображения полей удаленные/не удаленные
	void CheckQuery(const QString &QueryText); //Проверка запроса

private:
	PMetaClassTable *MetaTable;
	ISNamespace::QueryModelType ModelType;
	
	QString QuerySelectText; //Текст запроса на выборку
	QString QuerySelectSystemFields; //Список системных полей для запроса на выборку
	QString QuerySelectFields;
	QString QuerySelectFrom; //Отношение к таблице
	QString QuerySelectLeftJoin;
	QString QuerySelectIsDeleted;
	QString QueryWhereText; //Текст условия запроса
	
	QString QueryOrderText; //Сортировка
	QString OrderField; //Имя поля по которому идет сортировка
	QString OrderFieldDefault; //Поле по умолчанию по для сортировки
	Qt::SortOrder OrderSort; //Тип сортировки
	int Limit; //Вывод записей на странице
	int Offset; //Страница

	bool VisibleIsDeleted;

	ISNamespace::PeriodType PeriodType;
	ISRangeStruct PeriodRange;

	QString ClassAlias;
	QString ClassFilter;
	QString SearchFilter;

	QVariantMap Conditions;
	QStringMap ForeignFields;
};
//-----------------------------------------------------------------------------
