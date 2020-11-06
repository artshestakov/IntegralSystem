#include "ISUserHashForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
ISUserHashForm::ISUserHashForm(unsigned int user_id, const QString &user_fio)
	: ISInterfaceDialogForm(),
	EditLoginCurrent(nullptr),
	EditPasswordCurrent(nullptr),
	UserID(user_id),
	UserFIO(user_fio),
	LoginExist(false)
{
	ISTcpQuery qExistLogin(API_LOGIN_EXIST);
	qExistLogin.BindValue("UserID", UserID);
	if (qExistLogin.Execute())
	{
		LoginExist = qExistLogin.GetAnswer()["IsExist"].toBool();
	}
	else
	{
		ISMessageBox::ShowWarning(this, qExistLogin.GetErrorString());
		return;
	}

	setWindowTitle(LoginExist ? LANG("UserHashForm.Title.Change") : LANG("UserHashForm.Title.Creating"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QLabel *LabelUser = new QLabel(LANG("User") + ": " + UserFIO, this);
	LabelUser->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetBold(LabelUser, true);
	GetMainLayout()->addWidget(LabelUser);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	if (LoginExist) //Если логин уже был присвоен
	{
		//Текущий логин
		GetMainLayout()->addWidget(new QLabel(LANG("UserHashForm.CurrentLogin"), this), 0, Qt::AlignLeft);
		EditLoginCurrent = new ISLineEdit(this);
		EditLoginCurrent->SetRegExp(REG_EXP_LOGIN);
		GetMainLayout()->addWidget(EditLoginCurrent);

		//Текущий пароль
		GetMainLayout()->addWidget(new QLabel(LANG("UserHashForm.CurrentPassword"), this), 0, Qt::AlignLeft);
		EditPasswordCurrent = new ISLineEdit(this);
		EditPasswordCurrent->SetEchoMode(QLineEdit::Password);
		GetMainLayout()->addWidget(EditPasswordCurrent);
	}
	
	GetMainLayout()->addWidget(new QLabel(LANG("UserHashForm.Login"), this), 0, Qt::AlignLeft);
	EditLogin = new ISLineEdit(this);
	EditLogin->CreateHint(LANG("UserHashForm.Login.Hint"));
	EditLogin->SetRegExp(REG_EXP_LOGIN);
	GetMainLayout()->addWidget(EditLogin);

	GetMainLayout()->addWidget(new QLabel(LANG("UserHashForm.Password"), this), 0, Qt::AlignLeft);
	EditPassword = new ISLineEdit(this);
	EditPassword->SetEchoMode(QLineEdit::Password);
	GetMainLayout()->addWidget(EditPassword);

	GetMainLayout()->addWidget(new QLabel(LANG("UserHashForm.PasswordCheck"), this), 0, Qt::AlignLeft);
	EditPasswordCheck = new ISLineEdit(this);
	EditPasswordCheck->SetEchoMode(QLineEdit::Password);
	GetMainLayout()->addWidget(EditPasswordCheck);

	QLabel *LabelInfo = new QLabel(LANG("UserHashForm.LabelInfo"), this);
	LabelInfo->setWordWrap(true);
	ISGui::SetFontWidgetItalic(LabelInfo, true);
	GetMainLayout()->addWidget(LabelInfo);

	ButtonDialog = new ISButtonDialog(this);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISUserHashForm::Apply);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISUserHashForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISUserHashForm::~ISUserHashForm()
{

}
//-----------------------------------------------------------------------------
void ISUserHashForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	LoginExist ? EditLoginCurrent->SetFocus() : EditLogin->SetFocus();
}
//-----------------------------------------------------------------------------
void ISUserHashForm::EnterClicked()
{
	Apply();
}
//-----------------------------------------------------------------------------
void ISUserHashForm::Apply()
{
	QString Login = EditLogin->GetValue().toString(),
		Password = EditPassword->GetValue().toString(),
		PasswordCheck = EditPasswordCheck->GetValue().toString();
	if (Login.isEmpty() || Password.isEmpty() || PasswordCheck.isEmpty())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.OneFieldNotFill"));
		return;
	}

	if (Password != PasswordCheck)
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.PasswordsNotEqual"));
		return;
	}

	if (Password.size() < MINIMUM_PASSWORD_LENGHT)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.PasswordMinimumSize").arg(MINIMUM_PASSWORD_LENGHT));
		return;
	}

	ISTcpQuery qRegister(API_LOGIN_REGISTER);
	if (LoginExist)
	{
		qRegister.BindValue("HashOld", ISSystem::StringToSha256(EditLoginCurrent->GetValue().toString() + EditPasswordCurrent->GetValue().toString()));
	}
	qRegister.BindValue("Hash", ISSystem::StringToSha256(Login + Password));
	qRegister.BindValue("UserID", UserID);
	if (qRegister.Execute())
	{
		SetResult(true);
		close();
	}
	else
	{
		ISMessageBox::ShowWarning(this, qRegister.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
