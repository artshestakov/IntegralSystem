#include "ISTableDetailsForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
ISTableDetailsForm::ISTableDetailsForm(const QString &TableName, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("TableDetails") + ": " + TableName);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	FormLayout->addRow(LANG("Size") + ':', new QLabel(ISDatabase::Instance().GetTableSize(TableName), this));
	FormLayout->addRow(LANG("SizeWithIndices") + ':', new QLabel(ISDatabase::Instance().GetTableSizeWithIndices(TableName), this));
	FormLayout->addRow(LANG("RowCount") + ':', new QLabel(QString::number(ISDatabase::Instance().GetTableRowCount(TableName)), this));
}
//-----------------------------------------------------------------------------
ISTableDetailsForm::~ISTableDetailsForm()
{

}
//-----------------------------------------------------------------------------
