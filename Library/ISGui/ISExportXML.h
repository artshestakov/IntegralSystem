#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISExportXML : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportXML(PMetaTable *meta_table, QObject *parent = 0);
	virtual ~ISExportXML();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileXML;
};
//-----------------------------------------------------------------------------
