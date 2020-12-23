#include "ISPrintingEntity.h"
//-----------------------------------------------------------------------------
ISPrintingEntity::ISPrintingEntity()
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
