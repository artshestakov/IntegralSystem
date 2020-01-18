#include "ISPrintingBase.h"
//-----------------------------------------------------------------------------
ISPrintingBase::ISPrintingBase(ISPrintMetaReport *meta_report, int object_id, QObject *parent) : QObject(parent)
{
	MetaReport = meta_report;
	ObjectID = object_id;
}
//-----------------------------------------------------------------------------
ISPrintingBase::~ISPrintingBase()
{

}
//-----------------------------------------------------------------------------
QString ISPrintingBase::GetReportLocalName() const
{
	return ReportLocalName;
}
//-----------------------------------------------------------------------------
void ISPrintingBase::SetReportLocalName(const QString &report_local_name)
{
	ReportLocalName = report_local_name;
}
//-----------------------------------------------------------------------------
QString ISPrintingBase::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISPrintingBase::SetErrorString(const QString &error_string)
{
	ErrorString = error_string;
}
//-----------------------------------------------------------------------------
ISPrintMetaReport* ISPrintingBase::GetMetaReport()
{
	return MetaReport;
}
//-----------------------------------------------------------------------------
int ISPrintingBase::GetObjectID() const
{
	return ObjectID;
}
//-----------------------------------------------------------------------------
