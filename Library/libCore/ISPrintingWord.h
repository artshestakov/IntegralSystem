#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISPrintingBase.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISPrintingWord : public ISPrintingBase
{
	Q_OBJECT

public:
	ISPrintingWord(ISPrintMetaReport *meta_report, int object_id, QObject *parent = 0);
	virtual ~ISPrintingWord();

	bool Prepare() override;
	bool PrepareTempate() override;
	bool FillTemplate() override;
	bool PreviewDocument() override;
	bool Print() override;

private:
	QAxObject *WordApplication;
	QAxObject *Documents;
	QAxObject *Document;
	QAxObject *Bookmarks;
};
//-----------------------------------------------------------------------------
