#include "StdAfx.h"
#include "ISPrintMetaReportField.h"
//-----------------------------------------------------------------------------
ISPrintMetaReportField::ISPrintMetaReportField(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISPrintMetaReportField::~ISPrintMetaReportField()
{

}
//-----------------------------------------------------------------------------
void ISPrintMetaReportField::SetReplaceValue(const QString &replace_value)
{
	ReplaceValue = replace_value;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReportField::GetReplaceValue() const
{
	return ReplaceValue;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReportField::SetFieldQuery(const QString &field_query)
{
	FieldQuery = field_query;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReportField::GetFieldQuery() const
{
	return FieldQuery;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReportField::SetQueryName(const QString &query_name)
{
	QueryName = query_name;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReportField::GetQueryName() const
{
	return QueryName;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReportField::SetParameterName(const QString &parameter_name)
{
	ParameterName = parameter_name;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReportField::GetParameterName() const
{
	return ParameterName;
}
//-----------------------------------------------------------------------------
