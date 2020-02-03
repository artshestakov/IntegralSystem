#include "StdAfx.h"
#include "ISTransferOrganizationForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
ISTransferOrganizationForm::ISTransferOrganizationForm(int OrganizationCount, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QLabel *Label = new QLabel(LANG("SelectUserFromOrganizationWork").arg(OrganizationCount), this);
	Label->setWordWrap(true);
	GetMainLayout()->addWidget(Label);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	UserEdit = new ISUserEdit(this);
	FormLayout->addRow(LANG("User") + ':', UserEdit);

	DateEdit = new ISDateEdit(this);
	DateEdit->SetMinimumDate(DATE_TODAY);
	DateEdit->SetCheckEnable(Qt::Checked);
	DateEdit->SetVisibleCheck(false);
	FormLayout->addRow(LANG("Date") + ':', DateEdit);

	ISButtonDialog *ButtonDialog = new ISButtonDialog(this);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISTransferOrganizationForm::Select);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISTransferOrganizationForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISTransferOrganizationForm::~ISTransferOrganizationForm()
{

}
//-----------------------------------------------------------------------------
int ISTransferOrganizationForm::GetSelectedUserID() const
{
	return UserEdit->GetValue().toInt();
}
//-----------------------------------------------------------------------------
QString ISTransferOrganizationForm::GetSelectedUserName() const
{
	return UserEdit->GetCurrentText();
}
//-----------------------------------------------------------------------------
QDate ISTransferOrganizationForm::GetSelectedDate() const
{
	return DateEdit->GetValue().toDate();
}
//-----------------------------------------------------------------------------
void ISTransferOrganizationForm::Select()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
