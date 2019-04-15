#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISAssociationTypes
{
public:
	ISAssociationTypes();
	~ISAssociationTypes();

	void Insert(const QString &type_name, ISNamespace::FieldType meta_field_type, const QString &database_type, const QString &control_widget, const QString &search_operator_widget, bool search); //Добавить ассоциацию типа

	ISNamespace::FieldType GetTypeField(const QString &type_name); //Получить тип данных по мета-имени
	QString GetTypeDB(ISNamespace::FieldType field_type) const; //Получить наименование типа в базе данных
	QString GetTypeDB(const QString &type_name) const; //Получить наименование типа в базе данных
	QString GetControlWidgetFromType(ISNamespace::FieldType field_type) const; //Получить наименование класса для типа данных
	QString GetSearchOperatorWidget(ISNamespace::FieldType field_type) const; //Получить наименование класса оператора поиска
	bool GetSearch(ISNamespace::FieldType field_type) const; //Получить разрешение на поиск по типу данных

private:
	QVector<ISMetaDataType> Types;
};
//-----------------------------------------------------------------------------
