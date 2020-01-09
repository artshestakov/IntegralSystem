#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISExportDBF : public ISExportWorker
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
