#include "ISPrintingEntity.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_REPORT = PREPARE_QUERY("SELECT rprt_uid, rprt_type, rprt_tablename, rprt_localname, rprt_filetemplate, "
										 "rprt_parent, rprt_replacevalue, rprt_sqlquery "
										 "FROM _report "
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
void ISPrintingEntity::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &Report : VariantList)
	{
		QVariantMap ReportMap = Report.toMap();
		ISPrintMetaReport *PrintMetaReport = new ISPrintMetaReport();
		PrintMetaReport->SetType(ReportMap["Type"].toString());
		PrintMetaReport->TableName = ReportMap["Table"].toString();
		PrintMetaReport->LocalName = ReportMap["Local"].toString();
		PrintMetaReport->FileTemplate = ReportMap["File"].toString();

		QVariantMap ReportFieldMap = ReportMap["Fields"].toMap();
		for (const auto &MapItem : ReportFieldMap.toStdMap())
		{
			PrintMetaReport->Fields.emplace(MapItem.first, MapItem.second.toString());
		}
		Reports.emplace_back(PrintMetaReport);
	}
}
//-----------------------------------------------------------------------------
bool ISPrintingEntity::Initialize()
{
	return true;
	/*ISQuery qSelectReport(QS_REPORT);
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
				Reports.emplace_back(PrintMetaReport);
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
	return Result;*/
}
//-----------------------------------------------------------------------------
std::vector<ISPrintMetaReport*> ISPrintingEntity::GetReports(const QString &TableName)
{
	std::vector<ISPrintMetaReport*> Vector;
	for (ISPrintMetaReport *PrintMetaReport : Reports)
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
