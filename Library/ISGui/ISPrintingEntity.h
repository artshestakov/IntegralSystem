#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISPrintingEntity
{
public:
	static ISPrintingEntity& Instance();

	void Initialize(const QVariantList &VariantList);
	std::vector<ISPrintMetaReport*> GetReports(const QString &TableName);
	size_t GetCountReports(const QString &TableName);

private:
	ISPrintingEntity();
	~ISPrintingEntity();
	ISPrintingEntity(ISPrintingEntity const &) {};
	ISPrintingEntity& operator=(ISPrintingEntity const&) { return *this; };
	
private:
	std::vector<ISPrintMetaReport *> Reports;
};
//-----------------------------------------------------------------------------
