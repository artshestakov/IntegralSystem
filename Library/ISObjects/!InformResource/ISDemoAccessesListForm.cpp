#include "StdAfx.h"
#include "ISDemoAccessesListForm.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISDemoAccessesListForm::ISDemoAccessesListForm(QWidget *parent) : ISListBaseForm("DemoAccesses", parent)
{
	QLabel *Label = new QLabel(this);
	Label->setText(LOCALIZATION("AgreementDateTimeFilter") + ":");
	GetToolBar()->addWidget(Label);

	DateEdit = new ISDateEdit(this);
	connect(DateEdit, &ISDateEdit::ValueChange, this, &ISDemoAccessesListForm::AgreementDateTime);
	GetToolBar()->addWidget(DateEdit);
}
//-----------------------------------------------------------------------------
ISDemoAccessesListForm::~ISDemoAccessesListForm()
{

}
//-----------------------------------------------------------------------------
void ISDemoAccessesListForm::AgreementDateTime(const QVariant &value)
{
	if (value.isValid())
	{
		GetQueryModel()->SetClassFilter("dacc_agreementdatetime::DATE = :AgreementDateTime");
		GetQueryModel()->AddCondition(":AgreementDateTime", value);
	}
	else
	{
		GetQueryModel()->ClearClassFilter();
		GetQueryModel()->ClearConditions();
	}

	Update();
}
//-----------------------------------------------------------------------------
