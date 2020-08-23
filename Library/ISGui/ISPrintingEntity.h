#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISPrintingEntity
{
public:
	static ISPrintingEntity& Instance();

	QString GetErrorString() const;
	bool Initialize();
	std::vector<ISPrintMetaReport*>& GetReports(const QString &TableName);
	int GetCountReports(const QString &TableName);

private:
	ISPrintingEntity();
	~ISPrintingEntity();
	ISPrintingEntity(ISPrintingEntity const &) {};
	ISPrintingEntity& operator=(ISPrintingEntity const&) { return *this; };
	
private:
	QString ErrrorString;
	std::map<QString, std::vector<ISPrintMetaReport*>> Reports;
};
//-----------------------------------------------------------------------------
