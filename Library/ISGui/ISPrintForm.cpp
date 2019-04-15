#include "StdAfx.h"
#include "ISPrintForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISPrintingEntity.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISPrintForm::ISPrintForm(const QString &TableName, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	MetaReport = nullptr;

	setWindowTitle(LOCALIZATION("PrintForms"));
	setWindowIcon(BUFFER_ICONS("Print"));
	
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	connect(ListWidget, &QListWidget::itemClicked, [=] { ButtonPanel->SetApplyEnabled(true); });
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISPrintForm::Print);
	GetMainLayout()->addWidget(ListWidget);

	QVector<ISPrintMetaReport*> Vector = ISPrintingEntity::GetInstance().GetReports(TableName);
	for (int i = 0; i < Vector.count(); i++)
	{
		ISPrintMetaReport *MetaReport = Vector.at(i);

		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setText(MetaReport->GetLocalName());
		ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

		switch (MetaReport->GetType())
		{
		case ISNamespace::RT_Html: ListWidgetItem->setIcon(BUFFER_ICONS("Print.Type.Html")); break;
		case ISNamespace::RT_Word: ListWidgetItem->setIcon(BUFFER_ICONS("Print.Type.Word")); break;
		}

		Reports.insert(ListWidgetItem, MetaReport);
	}

	CheckEditPreview = new ISCheckEdit(this);
	CheckEditPreview->SetText(LOCALIZATION("PreviewPrint"));
	GetMainLayout()->addWidget(CheckEditPreview, 0, Qt::AlignLeft);

	CheckEditPDF = new ISCheckEdit(this);
	CheckEditPDF->SetText(LOCALIZATION("PrintOutput.PDF"));
	GetMainLayout()->addWidget(CheckEditPDF, 0, Qt::AlignLeft);

	ButtonPanel = new ISButtonDialog(this, LOCALIZATION("Print"));
	ButtonPanel->SetApplyEnabled(false);
	connect(ButtonPanel, &ISButtonDialog::Apply, this, &ISPrintForm::Print);
	connect(ButtonPanel, &ISButtonDialog::Close, this, &ISPrintForm::close);
	GetMainLayout()->addWidget(ButtonPanel, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISPrintForm::~ISPrintForm()
{

}
//-----------------------------------------------------------------------------
ISPrintMetaReport* ISPrintForm::GetMetaReport()
{
	return MetaReport;
}
//-----------------------------------------------------------------------------
bool ISPrintForm::GetPreview() const
{
	return CheckEditPreview->GetValue().toBool();
}
//-----------------------------------------------------------------------------
bool ISPrintForm::GetPDF() const
{
	return CheckEditPDF->GetValue().toBool();
}
//-----------------------------------------------------------------------------
void ISPrintForm::Print()
{
	MetaReport = Reports.value(ListWidget->currentItem());
	close();
}
//-----------------------------------------------------------------------------
