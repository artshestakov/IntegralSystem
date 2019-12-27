#include "StdAfx.h"
#include "PMetaClassForeign.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
PMetaClassForeign::PMetaClassForeign(QObject *parent) : PMetaClass(parent)
{
	
}
//-----------------------------------------------------------------------------
PMetaClassForeign::~PMetaClassForeign()
{

}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetFieldName(const QString &field)
{
	Field = field;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetFieldName() const
{
	return Field;
}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetForeignClass(const QString &foreign_class)
{
	ForeignClass = foreign_class;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetForeignClass() const
{
	return ForeignClass;
}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetForeignField(const QString &foreign_field)
{
	ForeignField = foreign_field;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetForeginField() const
{
	return ForeignField;
}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetForeignViewNameField(const QString &foreign_view_name)
{
	ForeignViewNameField = foreign_view_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetForeignViewNameField() const
{
	return ForeignViewNameField;
}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetOrderField(const QString &order_field)
{
	OrderField = order_field;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetOrderField() const
{
	return OrderField;
}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void PMetaClassForeign::SetSqlQuery(const QString &sql_query)
{
	SqlQuery = sql_query;
}
//-----------------------------------------------------------------------------
QString PMetaClassForeign::GetSqlQuery() const
{
	return SqlQuery;
}
//-----------------------------------------------------------------------------
