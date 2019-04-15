#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISExportHTML : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportHTML(QObject *parent = 0);
	virtual ~ISExportHTML();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileHTML;
};
//-----------------------------------------------------------------------------
