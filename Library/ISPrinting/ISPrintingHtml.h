#pragma once
//-----------------------------------------------------------------------------
#include "isprinting_global.h"
#include "ISPrintingBase.h"
//-----------------------------------------------------------------------------
class ISPRINTING_EXPORT ISPrintingHtml : public ISPrintingBase
{
	Q_OBJECT

	Q_PROPERTY(bool PDF WRITE SetPDF READ GetPDF)
	Q_PROPERTY(QString PathPDF WRITE SetPathPDF READ GetPathPDF)
	Q_PROPERTY(bool EditPreview WRITE SetEditPreview READ GetEditPreview)

public:
	ISPrintingHtml(ISPrintMetaReport *meta_report, int object_id, QObject *parent = 0);
	virtual ~ISPrintingHtml();

	bool Prepare() override;
	bool PrepareTempate() override;
	bool FillTemplate() override;
	bool PreviewDocument() override;
	bool Print() override;

	bool GetPDF() const;
	void SetPDF(bool pdf);

	QString GetPathPDF() const;
	void SetPathPDF(const QString &path_pdf);

	bool GetEditPreview() const;
	void SetEditPreview(bool edit_preview);

private:
	bool PDF;
	QString PathPDF;
	bool EditPreview;

	QString Html;
};
//-----------------------------------------------------------------------------
