#include "ISPhoneBaseEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISPhoneNumberParser.h"
//-----------------------------------------------------------------------------
ISPhoneBaseEdit::ISPhoneBaseEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	PhoneLineEdit = new ISPhoneLineEdit(this);
	PhoneLineEdit->SetIcon(ISNamespace::PIT_Standart);
	connect(PhoneLineEdit, &ISPhoneLineEdit::ClearClicked, this, &ISPhoneBaseEdit::Clear);
	connect(PhoneLineEdit, &ISPhoneLineEdit::textChanged, this, &ISPhoneBaseEdit::ValueChanged);
	connect(PhoneLineEdit, &ISPhoneLineEdit::Pasted, this, &ISPhoneBaseEdit::Paste);
	connect(this, &ISPhoneBaseEdit::DataChanged, this, &ISPhoneBaseEdit::PhoneChanged);
	AddWidgetEdit(PhoneLineEdit, this);

	ButtonCall = new ISServiceButton(this);
	ButtonCall->setVisible(false);
	ButtonCall->setToolTip(LANG("Call"));
	ButtonCall->setIcon(BUFFER_ICONS("CallPhone"));
	ButtonCall->setEnabled(false);
	ButtonCall->setFocusPolicy(Qt::NoFocus);
	connect(ButtonCall, &ISServiceButton::clicked, this, &ISPhoneBaseEdit::Call);
	AddWidgetToRight(ButtonCall);
}
//-----------------------------------------------------------------------------
ISPhoneBaseEdit::~ISPhoneBaseEdit()
{

}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::SetValue(const QVariant &value)
{
	PhoneLineEdit->setText(value.toString());
}
//-----------------------------------------------------------------------------
QVariant ISPhoneBaseEdit::GetValue() const
{
	QString PhoneString = ISPhoneNumberParser::RemoveNotDigits(PhoneLineEdit->text());
	if (PhoneString.length() == 10)
	{
		return PhoneString;
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::Clear()
{
	PhoneLineEdit->clear();
	PhoneLineEdit->ResetLastPosition();
}
//-----------------------------------------------------------------------------
bool ISPhoneBaseEdit::IsValid() const
{
	bool Result = true;
	QString PhoneString = ISPhoneNumberParser::RemoveNotDigits(PhoneLineEdit->text());
	if (PhoneString.length())
	{
		if (PhoneString.length() != 10)
		{
			Result = false;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::SetVisibleCall(bool Visible)
{
	ButtonCall->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::SetToolTipCall(const QString &ToolTip)
{
	ButtonCall->setToolTip(ToolTip);
}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::SetCursorCall(const QCursor &Cursor)
{
	ButtonCall->setCursor(Cursor);
}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::Call()
{

}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::PhoneChanged()
{
	disconnect(this, &ISPhoneBaseEdit::DataChanged, this, &ISPhoneBaseEdit::PhoneChanged);
	QString PhoneString = ISPhoneNumberParser::RemoveNotDigits(PhoneLineEdit->text());
	if (PhoneString.length() == 10) //Если номер телефона введен корректно
	{
		PhoneLineEdit->SetIcon(ISNamespace::PIT_TRUE);
		ButtonCall->setEnabled(true);
	}
	else //Номер телефона некорректный
	{
		ButtonCall->setEnabled(false);
		if (PhoneString.length() > 0 && PhoneString.length() < 10) //Номер телефона введен частично
		{
			PhoneLineEdit->SetIcon(ISNamespace::PIT_FALSE);
		}
		else if (PhoneString.length() == 0) //Номер телефона не введен вообще
		{
			PhoneLineEdit->SetIcon(ISNamespace::PIT_Standart);
		}
	}
	connect(this, &ISPhoneBaseEdit::DataChanged, this, &ISPhoneBaseEdit::PhoneChanged);
}
//-----------------------------------------------------------------------------
void ISPhoneBaseEdit::Paste(const QString &PastedString)
{
	SetValue(ISPhoneNumberParser::PasteEvent(PastedString));
}
//-----------------------------------------------------------------------------
