#include "StdAfx.h"
#include "ISPrintMetaReport.h"
#include "ISDefines.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISPrintMetaReport::ISPrintMetaReport(QObject *parent) : QObject(parent)
{
	Type = ISNamespace::RT_Unknown;
}
//-----------------------------------------------------------------------------
ISPrintMetaReport::~ISPrintMetaReport()
{

}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::SetSystem(bool system)
{
	System = system;
}
//-----------------------------------------------------------------------------
bool ISPrintMetaReport::GetSystem() const
{
	return System;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::SetType(const QString &type)
{
	if (type == REPORT_TYPE_HTML)
	{
		Type = ISNamespace::RT_Html;
	}
	else if (type == REPORT_TYPE_WORD)
	{
		Type = ISNamespace::RT_Word;
	}
	else
	{
		IS_ASSERT(false, QString("Inknown report type: %1").arg(type));
	}
}
//-----------------------------------------------------------------------------
ISNamespace::ReportType ISPrintMetaReport::GetType()
{
	return Type;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReport::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReport::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReport::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::SetFileTemplate(const QString &file_template)
{
	FileTemplate = file_template;
}
//-----------------------------------------------------------------------------
QString ISPrintMetaReport::GetFileTemplate() const
{
	return FileTemplate;
}
//-----------------------------------------------------------------------------
void ISPrintMetaReport::AddReportField(ISPrintMetaReportField *MetaReportField)
{
	Fields.append(MetaReportField);
}
//-----------------------------------------------------------------------------
QVector<ISPrintMetaReportField*> ISPrintMetaReport::GetFields()
{
	return Fields;
}
//-----------------------------------------------------------------------------
