#include "ISPassportForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtonDialog.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISPassportForm::ISPassportForm(const QString &PassportString, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("Passport.EditingPassport"));
	setWindowIcon(BUFFER_ICONS("Passport"));

	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditSeria = new ISLineEdit(this);
	EditSeria->SetMaxLength(4);
	EditSeria->SetPlaceholderText(LANG("Passport.Seria.Placeholder"));
	FormLayout->addRow(LANG("Passport.Seria") + ':', EditSeria);

	EditNumber = new ISLineEdit(this);
	EditNumber->SetMaxLength(6);
	EditNumber->SetPlaceholderText(LANG("Passport.Number.Placeholder"));
	FormLayout->addRow(LANG("Passport.Number") + ':', EditNumber);

	EditDateOfIssue = new ISDateEdit(this);
	FormLayout->addRow(LANG("Passport.DateOfIssue") + ':', EditDateOfIssue);

	EditIssuedBy = new ISTextEdit(this);
	EditIssuedBy->SetUppercase(true);
	FormLayout->addRow(LANG("Passport.IssuedBy") + ':', EditIssuedBy);

	EditDivisionCode = new ISDivisionCodeEdit(this);
	FormLayout->addRow(LANG("Passport.DivisionCode") + ':', EditDivisionCode);

	ISButtonDialog *ButtonDialog = new ISButtonDialog(this, LANG("Save"));
	ButtonDialog->SetApplyIcon(BUFFER_ICONS("Save"));
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISPassportForm::Apply);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISPassportForm::close);
	GetMainLayout()->addWidget(ButtonDialog);

	ParsePassportString(PassportString);
}
//-----------------------------------------------------------------------------
ISPassportForm::~ISPassportForm()
{

}
//-----------------------------------------------------------------------------
QString ISPassportForm::GetPassportString()
{
	QString Result;
	Result += EditSeria->GetValue().toString();
	Result += SYMBOL_SPACE_HIDE;
	Result += EditNumber->GetValue().toString();
	Result += SYMBOL_SPACE_HIDE;
	Result += EditDateOfIssue->GetValue().toDateTime().toString(DATE_FORMAT_V2);
	Result += SYMBOL_SPACE_HIDE;
	Result += EditIssuedBy->GetValue().toString();
	Result += SYMBOL_SPACE_HIDE;

	if (EditDivisionCode->GetValue() != "-")
	{
		Result += EditDivisionCode->GetValue().toString();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISPassportForm::AfterShowEvent()
{
	EditSeria->SetFocus();
}
//-----------------------------------------------------------------------------
void ISPassportForm::EnterClicked()
{
	Apply();
}
//-----------------------------------------------------------------------------
void ISPassportForm::Apply()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISPassportForm::ParsePassportString(const QString &PassportString)
{
	if (PassportString.length())
	{
		QStringList StringList = PassportString.split(SYMBOL_SPACE_HIDE);
		QString Seria = StringList.at(0);
		QString Number = StringList.at(1);
		QString DateOfIssue = StringList.at(2);
		QString IssuedBy = StringList.at(3);
		QString DivisionCode = StringList.at(4);

		if (Seria.length())
		{
			EditSeria->SetValue(Seria);
		}
		
		if (Number.length())
		{
			EditNumber->SetValue(Number);
		}

		if (DateOfIssue.length())
		{
			EditDateOfIssue->SetValue(QDate::fromString(DateOfIssue, DATE_FORMAT_V2));
		}

		if (IssuedBy.length())
		{
			EditIssuedBy->SetValue(IssuedBy);
		}

		if (DivisionCode.length())
		{
			EditDivisionCode->SetValue(DivisionCode);
		}
	}
}
//-----------------------------------------------------------------------------
