#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISButtonDialog.h"
#include "ISCheckEdit.h"
#include "ISPrintMetaReport.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPrintForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISPrintForm(const QString &TableName, QWidget *parent = 0);
	virtual ~ISPrintForm();

	ISPrintMetaReport* GetMetaReport();
	bool GetPreview() const;
	bool GetPDF() const;

protected:
	void Print();

private:
	ISListWidget *ListWidget;
	ISCheckEdit *CheckEditPreview;
	ISCheckEdit *CheckEditPDF;
	ISButtonDialog *ButtonPanel;

	QMap<QListWidgetItem*, ISPrintMetaReport*> Reports;
	ISPrintMetaReport *MetaReport;
};
//-----------------------------------------------------------------------------
