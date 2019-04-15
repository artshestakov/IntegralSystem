#pragma once
//-----------------------------------------------------------------------------
#include "isprinting_global.h"
#include "ISPrintMetaReport.h"
//-----------------------------------------------------------------------------
class ISPRINTING_EXPORT ISPrintingEntity : public QObject
{
	Q_OBJECT

public:
	ISPrintingEntity(const ISPrintingEntity &) = delete;
	ISPrintingEntity(ISPrintingEntity &&) = delete;
	ISPrintingEntity &operator=(const ISPrintingEntity &) = delete;
	ISPrintingEntity &operator=(ISPrintingEntity &&) = delete;
	~ISPrintingEntity();

	static ISPrintingEntity& GetInstance();

	QVector<ISPrintMetaReport*> GetReports(const QString &TableName);
	int GetCountReports(const QString &TableName);

protected:
	void Initialize();

private:
	ISPrintingEntity();
	
	QMap<QString, QVector<ISPrintMetaReport*>> Reports;
};
//-----------------------------------------------------------------------------
