#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISExportHTML : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportHTML(PMetaTable *meta_table, QObject *parent = 0);
	virtual ~ISExportHTML();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileHTML;
};
//-----------------------------------------------------------------------------
