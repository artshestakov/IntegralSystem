#include "ISPasswordEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISPassword.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISPasswordEdit::ISPasswordEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetEchoMode(QLineEdit::Password);
	SetPlaceholderText(LANG("Field.Password.PlaceholderText"));

	CheckPasswordVisible = new QCheckBox(this);
	CheckPasswordVisible->setFocusPolicy(Qt::NoFocus);
	CheckPasswordVisible->setCursor(CURSOR_POINTING_HAND);
	CheckPasswordVisible->setToolTip(LANG("ShowHidePassword"));
	connect(CheckPasswordVisible, &QCheckBox::stateChanged, this, &ISPasswordEdit::PasswordVisibleChanged);
	AddWidgetToLeft(CheckPasswordVisible);

	ButtonGeneratePassword = new ISServiceButton(this);
	ButtonGeneratePassword->setVisible(true);
	ButtonGeneratePassword->setToolTip(LANG("GeneratePassword") + "...");
	ButtonGeneratePassword->setIcon(BUFFER_ICONS("PasswordGenerate"));
	connect(ButtonGeneratePassword, &ISPushButton::clicked, this, &ISPasswordEdit::GeneratePassword);
	AddWidgetToRight(ButtonGeneratePassword);

	SetVisibleGenerate(false);
}
//-----------------------------------------------------------------------------
ISPasswordEdit::~ISPasswordEdit()
{

}
//-----------------------------------------------------------------------------
void ISPasswordEdit::SetVisibleGenerate(bool Visible)
{
	ButtonGeneratePassword->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISPasswordEdit::SetVisibleCheckBox(bool Visible)
{
	CheckPasswordVisible->setVisible(Visible);
}
//-----------------------------------------------------------------------------
void ISPasswordEdit::PasswordVisibleChanged(int State)
{
	switch (State)
	{
	case Qt::Checked: SetEchoMode(QLineEdit::Normal); break;
	case Qt::Unchecked: SetEchoMode(QLineEdit::Password); break;
	}

	SetFocus();
}
//-----------------------------------------------------------------------------
void ISPasswordEdit::GeneratePassword()
{
	SetValue(ISPassword::GeneratePassword());
}
//-----------------------------------------------------------------------------
