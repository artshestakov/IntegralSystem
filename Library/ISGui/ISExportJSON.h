#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISExportJSON : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportJSON(PMetaTable *meta_table, ISTcpModel *tcp_model, QObject *parent = 0);
	virtual ~ISExportJSON();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileJSON;
};
//-----------------------------------------------------------------------------
