#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISExportCSV : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportCSV(QObject *parent = 0);
	virtual ~ISExportCSV();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileCSV;
};
//-----------------------------------------------------------------------------
