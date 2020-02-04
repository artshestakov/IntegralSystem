#pragma once
#ifndef _PMETACLASSFOREIGN_H_INCLUDED
#define _PMETACLASSFOREIGN_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassForeign : public PMetaClass
{
	Q_OBJECT

	Q_PROPERTY(QString Field READ GetFieldName WRITE SetFieldName)
	Q_PROPERTY(QString ForeignClass READ GetForeignClass WRITE SetForeignClass)
	Q_PROPERTY(QString ForeignField READ GetForeginField WRITE SetForeignField)
	Q_PROPERTY(QString ForeignViewNameField READ GetForeignViewNameField WRITE SetForeignViewNameField)
	Q_PROPERTY(QString OrderField READ GetOrderField WRITE SetOrderField)
	Q_PROPERTY(QString TableName READ GetTableName WRITE SetTableName)
	Q_PROPERTY(QString SqlQuery READ GetSqlQuery WRITE SetSqlQuery)

public:
	PMetaClassForeign(QObject *parent = 0);
	virtual ~PMetaClassForeign();

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
	QString Field; //Поле, на которое устанавливается внешний ключ
	QString ForeignClass; //На какую таблицу ссылкается внешний ключ
	QString ForeignField; //На какое поле ссылкается внешний ключ
	QString ForeignViewNameField; //Какое поле (поля) отображать в запросе на выборку
	QString OrderField; //Поле по которому будет происходить сортировка

	QString TableName; //Таблица, содержащая поле, на которое устанавливается внешний ключ
	QString SqlQuery; //Запрос на выбору по внешнему ключу
};
//-----------------------------------------------------------------------------
#endif
