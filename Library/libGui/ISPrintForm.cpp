#include "ISPrintForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISPrintingEntity.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISPrintForm::ISPrintForm(const QString &TableName, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	MetaReport = nullptr;

	setWindowTitle(LANG("PrintForms"));
	setWindowIcon(BUFFER_ICONS("Print"));
	
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);

	ListWidget = new ISListWidget(this);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	connect(ListWidget, &QListWidget::itemClicked, [=] { ButtonPanel->SetApplyEnabled(true); });
	connect(ListWidget, &QListWidget::itemDoubleClicked, this, &ISPrintForm::Print);
	GetMainLayout()->addWidget(ListWidget);

	QVector<ISPrintMetaReport*> Vector = ISPrintingEntity::GetInstance().GetReports(TableName);
	for (int i = 0; i < Vector.count(); ++i)
	{
		ISPrintMetaReport *MetaReport = Vector.at(i);

		QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
		ListWidgetItem->setText(MetaReport->LocalName);
		ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

		switch (MetaReport->Type)
		{
		case ISNamespace::RT_Html: ListWidgetItem->setIcon(BUFFER_ICONS("Print.Type.Html")); break;
		case ISNamespace::RT_Word: ListWidgetItem->setIcon(BUFFER_ICONS("Print.Type.Word")); break;
		}

		Reports.insert(ListWidgetItem, MetaReport);
	}

	CheckEditPreview = new ISCheckEdit(this);
	CheckEditPreview->SetText(LANG("PreviewPrint"));
	GetMainLayout()->addWidget(CheckEditPreview, 0, Qt::AlignLeft);

	CheckEditPDF = new ISCheckEdit(this);
	CheckEditPDF->SetText(LANG("PrintOutput.PDF"));
	GetMainLayout()->addWidget(CheckEditPDF, 0, Qt::AlignLeft);

	ButtonPanel = new ISButtonDialog(this, LANG("Print"));
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
