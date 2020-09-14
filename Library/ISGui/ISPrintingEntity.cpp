#include "ISPrintingEntity.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_REPORT = PREPARE_QUERY("SELECT rprt_uid, rprt_type, rprt_tablename, rprt_localname, rprt_filetemplate, "
										 "rprt_parent, rprt_replacevalue, rprt_sqlquery "
										 "FROM _report "
										 "WHERE NOT rprt_isdeleted "
										 "ORDER BY rprt_id");
//-----------------------------------------------------------------------------
ISPrintingEntity::ISPrintingEntity()
	: ErrrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISPrintingEntity::~ISPrintingEntity()
{

}
//-----------------------------------------------------------------------------
ISPrintingEntity& ISPrintingEntity::Instance()
{
	static ISPrintingEntity PrintingEntity;
	return PrintingEntity;
}
//-----------------------------------------------------------------------------
QString ISPrintingEntity::GetErrorString() const
{
	return ErrrorString;
}
//-----------------------------------------------------------------------------
bool ISPrintingEntity::Initialize()
{
	ISQuery qSelectReport(QS_REPORT);
	bool Result = qSelectReport.Execute();
	if (Result)
	{
		while (qSelectReport.Next())
		{
			ISUuid UID = qSelectReport.ReadColumn("rprt_uid"),
				Parent = qSelectReport.ReadColumn("rprt_parent");
			if (Parent.isEmpty())
			{
				ISPrintMetaReport *PrintMetaReport = new ISPrintMetaReport();
				PrintMetaReport->SetType(qSelectReport.ReadColumn("rprt_type").toString());
				PrintMetaReport->TableName = qSelectReport.ReadColumn("rprt_tablename").toString();
				PrintMetaReport->LocalName = qSelectReport.ReadColumn("rprt_localname").toString();
				PrintMetaReport->FileTemplate = qSelectReport.ReadColumn("rprt_filetemplate").toString();
				Reports[UID] = PrintMetaReport;
			}
			else
			{
				Reports[Parent]->Fields.emplace_back(new ISPrintMetaReportField
				{
					qSelectReport.ReadColumn("rprt_replacevalue").toString(),
					qSelectReport.ReadColumn("rprt_sqlquery").toString()
				});
			}
		}
	}
	else
	{
		ErrrorString = qSelectReport.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
std::vector<ISPrintMetaReport*> ISPrintingEntity::GetReports(const QString &TableName)
{
	std::vector<ISPrintMetaReport*> Vector,
		Temp = ISAlgorithm::ConvertMapToValues(Reports);
	for (ISPrintMetaReport *PrintMetaReport : Temp)
	{
		if (PrintMetaReport->TableName == TableName)
		{
			Vector.emplace_back(PrintMetaReport);
		}
	}
	return Vector;
}
//-----------------------------------------------------------------------------
size_t ISPrintingEntity::GetCountReports(const QString &TableName)
{
	return GetReports(TableName).size();
}
//-----------------------------------------------------------------------------
