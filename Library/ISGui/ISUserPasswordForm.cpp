#include "ISUserPasswordForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISTcpQuery.h"
#include "ISPopupMessage.h"
//-----------------------------------------------------------------------------
ISUserPasswordForm::ISUserPasswordForm(unsigned int user_id, const QString &user_fio, const QString &user_login)
	: ISInterfaceDialogForm(),
	EditPasswordCurrent(nullptr),
	UserID(user_id),
	UserFIO(user_fio),
	UserLogin(user_login)
{
	ISTcpQuery qUserPasswordExist(API_USER_PASSWORD_EXIST);
	qUserPasswordExist.BindValue("UserID", UserID);
	if (!qUserPasswordExist.Execute())
	{
		ISMessageBox::ShowWarning(this, qUserPasswordExist.GetErrorString());
		return;
	}
	PasswordExist = qUserPasswordExist.GetAnswer()["IsExist"].toBool();

	setWindowTitle(PasswordExist ? LANG("UserPasswordForm.Title.Change") : LANG("UserPasswordForm.Title.Creating"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelUser = new QLabel(LANG("User") + ": " + UserFIO, this);
	LabelUser->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetBold(LabelUser, true);
	GetMainLayout()->addWidget(LabelUser);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	if (PasswordExist) //≈сли пароль существует - добавл€ем поле с текущим паролем дл€ изменени€
	{
		//“екущий пароль
		GetMainLayout()->addWidget(new QLabel(LANG("UserPasswordForm.CurrentPassword"), this), 0, Qt::AlignLeft);
		EditPasswordCurrent = new ISLineEdit(this);
		EditPasswordCurrent->SetEchoMode(QLineEdit::Password);
		GetMainLayout()->addWidget(EditPasswordCurrent);
	}
	
	GetMainLayout()->addWidget(new QLabel(LANG("UserPasswordForm.Password"), this), 0, Qt::AlignLeft);
	EditPassword = new ISLineEdit(this);
	EditPassword->SetEchoMode(QLineEdit::Password);
	GetMainLayout()->addWidget(EditPassword);

	GetMainLayout()->addWidget(new QLabel(LANG("UserPasswordForm.PasswordCheck"), this), 0, Qt::AlignLeft);
	EditPasswordCheck = new ISLineEdit(this);
	EditPasswordCheck->SetEchoMode(QLineEdit::Password);
	GetMainLayout()->addWidget(EditPasswordCheck);

	ButtonDialog = new ISButtonDialog(this);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISUserPasswordForm::Apply);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISUserPasswordForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISUserPasswordForm::~ISUserPasswordForm()
{

}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	PasswordExist ? EditPasswordCurrent->SetFocus() : EditPassword->SetFocus();
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::EnterClicked()
{
	Apply();
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::Apply()
{
	QString PasswordCurrent, Password, PasswordCheck;
	if (PasswordExist)
	{
		PasswordCurrent = EditPasswordCurrent->GetValue().toString();
		Password = EditPassword->GetValue().toString();
		PasswordCheck = EditPasswordCheck->GetValue().toString();
		if (PasswordCurrent.isEmpty() || Password.isEmpty() || PasswordCheck.isEmpty())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.OneFieldNotFill"));
			return;
		}
	}
	else
	{
		Password = EditPassword->GetValue().toString();
		PasswordCheck = EditPasswordCheck->GetValue().toString();
		if (Password.isEmpty() || PasswordCheck.isEmpty())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.OneFieldNotFill"));
			return;
		}
	}

	//ѕроверка сложности парол€
	if (!ISAlgorithm::PasswordVerification(Password))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.PasswordVerification"), LANG("Message.Warning.PasswordVerification.Detailed"));
		EditPassword->BlinkRed();
		return;
	}

	if (Password != PasswordCheck)
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.PasswordsNotEqual"));
		return;
	}

	ISTcpQuery qUserPassword(PasswordExist ? API_USER_PASSWORD_EDIT : API_USER_PASSWORD_CREATE);
	qUserPassword.BindValue("UserID", UserID);
	if (PasswordExist)
	{
		qUserPassword.BindValue("HashOld", ISSystem::StringToSha256(UserLogin + PasswordCurrent));
	}
	qUserPassword.BindValue("Hash", ISSystem::StringToSha256(UserLogin + Password));
	if (qUserPassword.Execute())
	{
		ISPopupMessage::ShowNotification(PasswordExist ? LANG("UserPasswordForm.EditedPassword") : LANG("UserPasswordForm.CreatedPassword"));
		SetResult(true);
		close();
	}
	else
	{
		ISMessageBox::ShowWarning(this, qUserPassword.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
