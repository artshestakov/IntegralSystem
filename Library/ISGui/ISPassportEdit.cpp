#include "ISPassportEdit.h"
#include "ISButtons.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISPassportForm.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISPassportEdit::ISPassportEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetReadOnly(true);
	Clear();

	ISServiceButton *ButtonEdit = new ISServiceButton(this);
	ButtonEdit->setToolTip(LANG("Passport.EditPassport"));
	ButtonEdit->setIcon(BUFFER_ICONS("Passport"));
	ButtonEdit->setFocusPolicy(Qt::NoFocus);
	connect(ButtonEdit, &ISServiceButton::clicked, this, &ISPassportEdit::Edit);
	AddWidgetToRight(ButtonEdit);
}
//-----------------------------------------------------------------------------
ISPassportEdit::~ISPassportEdit()
{

}
//-----------------------------------------------------------------------------
void ISPassportEdit::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		ISLineEdit::SetValue(PreparePassport(value.toString()));
	}
	else
	{
		GetLineEdit()->setText(LANG("NotFill"));
	}

	PassportString = value.toString();
}
//-----------------------------------------------------------------------------
QVariant ISPassportEdit::GetValue() const
{
	return PassportString;
}
//-----------------------------------------------------------------------------
void ISPassportEdit::Clear()
{
	disconnect(GetLineEdit(), &ISQLineEdit::textChanged, this, &ISPassportEdit::ValueChanged);
	GetLineEdit()->setText(LANG("NotFill"));
	connect(GetLineEdit(), &ISQLineEdit::textChanged, this, &ISPassportEdit::ValueChanged);

	PassportString.clear();
}
//-----------------------------------------------------------------------------
void ISPassportEdit::Edit()
{
	ISGui::SetWaitGlobalCursor(true);
	ISPassportForm PassportForm(PassportString);
	ISGui::SetWaitGlobalCursor(false);
	if (PassportForm.Exec())
	{
		PassportString = PassportForm.GetPassportString();
		SetValue(PassportString);
		SetFocus();
	}
}
//-----------------------------------------------------------------------------
QString ISPassportEdit::PreparePassport(const QString &passport_string)
{
	QString Result;

	if (passport_string.length())
	{
		QStringList StringList = passport_string.split(ISDefines::Core::SYMBOL_SPACE_HIDE);
		QString Seria = StringList.at(0);
		QString Number = StringList.at(1);
		QString DateOfIssue = StringList.at(2);
		QString Issued = StringList.at(3);
		QString DivisionCode = StringList.at(4);

		if (Seria.length())
		{
			Result += LANG("Passport.Seria") + ": " + Seria + SYMBOL_SPACE;
		}

		if (Number.length())
		{
			Result += LANG("Passport.Number") + ": " + Number + SYMBOL_SPACE;
		}

		if (DateOfIssue.length())
		{
			Result += LANG("Passport.DateOfIssue") + ": " + DateOfIssue + SYMBOL_SPACE;
		}

		if (Issued.length())
		{
			Result += LANG("Passport.Issued") + ": " + Issued + SYMBOL_SPACE;
		}

		if (DivisionCode.length())
		{
			Result += LANG("Passport.DivisionCode") + ": " + DivisionCode + SYMBOL_SPACE;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
