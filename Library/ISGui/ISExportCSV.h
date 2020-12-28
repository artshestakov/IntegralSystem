#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISExportCSV : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportCSV(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportCSV();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileCSV;
};
//-----------------------------------------------------------------------------
