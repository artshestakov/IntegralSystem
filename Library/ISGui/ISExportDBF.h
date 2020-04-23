#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExportWorker.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISExportDBF : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportDBF(QObject *parent = 0);
	virtual ~ISExportDBF();

	bool Prepare() override;
	bool Export() override;

private:
	ISUuid ConnectionName;
};
//-----------------------------------------------------------------------------
