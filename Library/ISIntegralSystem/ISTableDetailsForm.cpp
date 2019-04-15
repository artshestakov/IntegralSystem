#include "StdAfx.h"
#include "ISTableDetailsForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
ISTableDetailsForm::ISTableDetailsForm(const QString &TableName, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LOCALIZATION("TableDetails") + ": " + TableName);
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	FormLayout->addRow(LOCALIZATION("Size") + ":", new QLabel(ISDatabase::GetInstance().GetTableSize(TableName), this));
	FormLayout->addRow(LOCALIZATION("SizeWithIndices") + ":", new QLabel(ISDatabase::GetInstance().GetTableSizeWithIndices(TableName), this));
	FormLayout->addRow(LOCALIZATION("RowCount") + ":", new QLabel(QString::number(ISDatabase::GetInstance().GetTableRowCount(TableName)), this));
}
//-----------------------------------------------------------------------------
ISTableDetailsForm::~ISTableDetailsForm()
{

}
//-----------------------------------------------------------------------------
