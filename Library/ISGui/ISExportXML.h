#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISExportWorker.h"
//-----------------------------------------------------------------------------
class ISExportXML : public ISExportWorker
{
	Q_OBJECT

public:
	ISExportXML(QObject *parent = 0);
	virtual ~ISExportXML();

	bool Prepare() override;
	bool Export() override;

private:
	QFile *FileXML;
};
//-----------------------------------------------------------------------------
