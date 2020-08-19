#include "ISPrintingEntity.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_REPORT = PREPARE_QUERY("SELECT rprt_uid, rprt_system, rprt_type, rprt_tablename, rprt_name, rprt_localname, rprt_filetemplate "
										 "FROM _report "
										 "WHERE NOT rprt_isdeleted "
										 "ORDER BY rprt_id");
//-----------------------------------------------------------------------------
static QString QS_REPORT_FIELDS = PREPARE_QUERY("SELECT rpfl_replacevalue, rpfl_fieldquery, rpfl_queryname, rpfl_parametername "
												"FROM _reportfields "
												"WHERE NOT rpfl_isdeleted "
												"AND rpfl_report = :ReportUID");
//-----------------------------------------------------------------------------
ISPrintingEntity::ISPrintingEntity()
{
	Initialize();
}
//-----------------------------------------------------------------------------
ISPrintingEntity::~ISPrintingEntity()
{

}
//-----------------------------------------------------------------------------
ISPrintingEntity& ISPrintingEntity::GetInstance()
{
	static ISPrintingEntity PrintingEntity;
	return PrintingEntity;
}
//-----------------------------------------------------------------------------
QVector<ISPrintMetaReport*> ISPrintingEntity::GetReports(const QString &TableName)
{
	return Reports.value(TableName);
}
//-----------------------------------------------------------------------------
int ISPrintingEntity::GetCountReports(const QString &TableName)
{
	return Reports.value(TableName).count();
}
//-----------------------------------------------------------------------------
void ISPrintingEntity::Initialize()
{
	ISQuery qSelectReport(QS_REPORT);
	if (qSelectReport.Execute())
	{
		while (qSelectReport.Next())
		{
			ISUuid ReportUID = qSelectReport.ReadColumn("rprt_uid");
			QString TableName = qSelectReport.ReadColumn("rprt_tablename").toString();
			
			ISPrintMetaReport *MetaReport = new ISPrintMetaReport();
			MetaReport->System = qSelectReport.ReadColumn("rprt_system").toBool();
			//MetaReport->SetType(qSelectReport.ReadColumn("rprt_type").toString());
			MetaReport->Name = qSelectReport.ReadColumn("rprt_name").toString();
			MetaReport->LocalName = qSelectReport.ReadColumn("rprt_localname").toString();
			MetaReport->FileTemplate = qSelectReport.ReadColumn("rprt_filetemplate").toString();

			ISQuery qSelectReportFields(QS_REPORT_FIELDS);
			qSelectReportFields.BindValue(":ReportUID", ReportUID);
			if (qSelectReportFields.Execute())
			{
				while (qSelectReportFields.Next())
				{
					ISPrintMetaReportField *MetaReportField = new ISPrintMetaReportField();
					MetaReportField->ReplaceValue = qSelectReportFields.ReadColumn("rpfl_replacevalue").toString();
					MetaReportField->FieldQuery = qSelectReportFields.ReadColumn("rpfl_fieldquery").toString();
					MetaReportField->QueryName = qSelectReportFields.ReadColumn("rpfl_queryname").toString();
					MetaReportField->ParameterName = qSelectReportFields.ReadColumn("rpfl_parametername").toString();
					MetaReport->Fields.append(MetaReportField);
				}
			}

			QVector<ISPrintMetaReport*> Vector = Reports.value(TableName);
			Vector.append(MetaReport);
			Reports.insert(TableName, Vector);
		}
	}
}
//-----------------------------------------------------------------------------
