#include "StdAfx.h"
#include "ISPrintingEntity.h"
#include "ISQuery.h"
#include "ISPrintMetaReportField.h"
#include "ISCountingTime.h"
#include "ISDebug.h"
#include "ISSystem.h"
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
ISPrintingEntity::ISPrintingEntity() : QObject()
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
	QVector<ISPrintMetaReport*> Vector = Reports.value(TableName);
	return Vector;
}
//-----------------------------------------------------------------------------
int ISPrintingEntity::GetCountReports(const QString &TableName)
{
	QVector<ISPrintMetaReport*> Vector = Reports.value(TableName);
	int Count = Vector.count();
	return Count;
}
//-----------------------------------------------------------------------------
void ISPrintingEntity::Initialize()
{
	ISCountingTime CountingTime;

	ISQuery qSelectReport(QS_REPORT);
	if (qSelectReport.Execute())
	{
		while (qSelectReport.Next())
		{
			ISUuid ReportUID = qSelectReport.ReadColumn("rprt_uid");
			bool System = qSelectReport.ReadColumn("rprt_system").toBool();
			QString Type = qSelectReport.ReadColumn("rprt_type").toString();
			QString TableName = qSelectReport.ReadColumn("rprt_tablename").toString();
			QString Name = qSelectReport.ReadColumn("rprt_name").toString();
			QString LocalName = qSelectReport.ReadColumn("rprt_localname").toString();
			QString FileTemplate = qSelectReport.ReadColumn("rprt_filetemplate").toString();

			ISPrintMetaReport *MetaReport = new ISPrintMetaReport(this);
			MetaReport->SetSystem(System);
			MetaReport->SetType(Type);
			MetaReport->SetName(Name);
			MetaReport->SetLocalName(LocalName);
			MetaReport->SetFileTemplate(FileTemplate);

			ISQuery qSelectReportFields(QS_REPORT_FIELDS);
			qSelectReportFields.BindValue(":ReportUID", ReportUID);
			if (qSelectReportFields.Execute())
			{
				while (qSelectReportFields.Next())
				{
					QString ReplaceValue = qSelectReportFields.ReadColumn("rpfl_replacevalue").toString();
					QString FieldQuery = qSelectReportFields.ReadColumn("rpfl_fieldquery").toString();
					QString QueryName = qSelectReportFields.ReadColumn("rpfl_queryname").toString();
					QString ParameterName = qSelectReportFields.ReadColumn("rpfl_parametername").toString();

					ISPrintMetaReportField *MetaReportField = new ISPrintMetaReportField(MetaReport);
					MetaReportField->SetReplaceValue(ReplaceValue);
					MetaReportField->SetFieldQuery(FieldQuery);
					MetaReportField->SetQueryName(QueryName);
					MetaReportField->SetParameterName(ParameterName);
					MetaReport->AddReportField(MetaReportField);
				}
			}

			QVector<ISPrintMetaReport*> Vector = Reports.value(TableName);
			Vector.append(MetaReport);

			Reports.insert(TableName, Vector);
		}
	}

	ISDebug::ShowDebugString(QString("Initialized Printing %1 msec").arg(CountingTime.GetElapsed()));
}
//-----------------------------------------------------------------------------
