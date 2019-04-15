#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISExportDBF : public ISExportWorker
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
