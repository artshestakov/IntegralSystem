#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISPrintingBase : public QObject
{
	Q_OBJECT

signals:
	void SetVisibleDialog(bool Visible);
	void Message(const QString &Message);

public:
	ISPrintingBase(ISPrintMetaReport *meta_report, int object_id, QObject *parent = 0);
	virtual ~ISPrintingBase();

	virtual bool Prepare() = 0; //Подготовка
	virtual bool PrepareTempate() = 0; //Подготовка шаблона
	virtual bool FillTemplate() = 0; //Запонление шаблона
	virtual bool PreviewDocument() = 0; //Предварительный просмотр
	virtual bool Print() = 0; //Печать

	QString GetReportLocalName() const;
	void SetReportLocalName(const QString &report_local_name);

	QString GetErrorString() const;

protected:
	void SetErrorString(const QString &error_string);
	ISPrintMetaReport* GetMetaReport();
	int GetObjectID() const;

private:
	QString ErrorString;
	QString ReportLocalName;

	ISPrintMetaReport *MetaReport;
	int ObjectID;
};
//-----------------------------------------------------------------------------
