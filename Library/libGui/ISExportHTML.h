#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISExportHTML : public ISExportWorker
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
