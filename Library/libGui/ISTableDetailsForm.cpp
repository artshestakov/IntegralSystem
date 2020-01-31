#include "StdAfx.h"
#include "ISTableDetailsForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
ISTableDetailsForm::ISTableDetailsForm(const QString &TableName, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("TableDetails") + ": " + TableName);
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	FormLayout->addRow(LANG("Size") + ":", new QLabel(ISDatabase::GetInstance().GetTableSize(TableName), this));
	FormLayout->addRow(LANG("SizeWithIndices") + ":", new QLabel(ISDatabase::GetInstance().GetTableSizeWithIndices(TableName), this));
	FormLayout->addRow(LANG("RowCount") + ":", new QLabel(QString::number(ISDatabase::GetInstance().GetTableRowCount(TableName)), this));
}
//-----------------------------------------------------------------------------
ISTableDetailsForm::~ISTableDetailsForm()
{

}
//-----------------------------------------------------------------------------
