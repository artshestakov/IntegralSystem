#pragma once
//-----------------------------------------------------------------------------
#include "isprinting_global.h"
#include "ISPrintMetaReportField.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISPRINTING_EXPORT ISPrintMetaReport : public QObject
{
	Q_OBJECT

public:
	ISPrintMetaReport(QObject *parent = 0);
	virtual ~ISPrintMetaReport();

	void SetSystem(bool system);
	bool GetSystem() const;

	void SetType(const QString &type);
	ISNamespace::ReportType GetType();

	void SetTableName(const QString &table_name);
	QString GetTableName() const;

	void SetName(const QString &name);
	QString GetName() const;

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetFileTemplate(const QString &file_template);
	QString GetFileTemplate() const;

	void AddReportField(ISPrintMetaReportField *MetaReportField);
	QVector<ISPrintMetaReportField*> GetFields();

private:
	bool System;
	ISNamespace::ReportType Type;
	QString TableName;
	QString Name;
	QString LocalName;
	QString FileTemplate;

	QVector<ISPrintMetaReportField*> Fields;
};
//-----------------------------------------------------------------------------
