#include "StdAfx.h"
#include "ISUserPasswordForm.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISSystem.h"
#include "ISProtocol.h"
//-----------------------------------------------------------------------------
static QString QS_USER = PREPARE_QUERY("SELECT usrs_login, userfullname(:UserID) FROM _users WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_AUTHID = PREPARE_QUERY("SELECT rolpassword "
										 "FROM pg_authid "
										 "WHERE rolname = :Login");
//-----------------------------------------------------------------------------
static QString QA_PASSWORD = "ALTER ROLE %1 WITH ENCRYPTED PASSWORD '%2'";
//-----------------------------------------------------------------------------
static QString QI_USER_PASSWORD_CHANGED = PREPARE_QUERY("INSERT INTO _userpasswordchanged(upcg_user, upcg_whouser) "
														"VALUES(:User, :WhoUser)");
//-----------------------------------------------------------------------------
ISUserPasswordForm::ISUserPasswordForm(int user_id, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	ISQuery qSelectUser(QS_USER);
	qSelectUser.BindValue(":UserID", user_id);
	if (qSelectUser.ExecuteFirst())
	{
		UserID = user_id;
		Login = qSelectUser.ReadColumn("usrs_login").toString();
		UserFullName = qSelectUser.ReadColumn("userfullname").toString();
	}

	setWindowTitle(LOCALIZATION("ChangePassword"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	GetMainLayout()->addWidget(new QLabel(LOCALIZATION("User") + ": " + UserFullName, this));

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS("Keyboard").pixmap(SIZE_25_25));
	Layout->addWidget(LabelIcon);

	LabelText = new QLabel(LOCALIZATION("EnterThePassword"), this);
	Layout->addWidget(LabelText);

	Layout->addStretch();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditCurrentPassword = new ISLineEdit(this);
	EditCurrentPassword->SetEchoMode(QLineEdit::Password);
	FormLayout->addRow(LOCALIZATION("CurrentPassword") + ":", EditCurrentPassword);

	EditPassword = new ISLineEdit(this);
	EditPassword->SetEchoMode(QLineEdit::Password);
	connect(EditPassword, &ISLineEdit::DataChanged, this, &ISUserPasswordForm::PasswordChecked);
	FormLayout->addRow(LOCALIZATION("Password") + ":", EditPassword);

	EditPasswordCheck = new ISLineEdit(this);
	EditPasswordCheck->SetEchoMode(QLineEdit::Password);
	connect(EditPasswordCheck, &ISLineEdit::DataChanged, this, &ISUserPasswordForm::PasswordChecked);
	FormLayout->addRow(LOCALIZATION("PasswordCheck") + ":", EditPasswordCheck);

	ISPasswordWidthWidget *PasswordWidthWidget = new ISPasswordWidthWidget(this);
	connect(EditPassword, &ISLineEdit::ValueChange, PasswordWidthWidget, &ISPasswordWidthWidget::SetPassword);
	GetMainLayout()->addWidget(PasswordWidthWidget);

	ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISUserPasswordForm::ChangePassword);
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
	EditCurrentPassword->SetFocus();
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::EnterClicked()
{
	ChangePassword();
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::ChangePassword()
{
	if (EditPassword->GetValue().toString().length() < MINIMUM_PASSWORD_LENGHT)
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.PasswordLenghtSmall").arg(MINIMUM_PASSWORD_LENGHT));
		return;
	}

	ISSystem::SetWaitGlobalCursor(true);

	ISQuery qSelectAuth(QS_AUTHID);
	qSelectAuth.BindValue(":Login", Login);
	if (qSelectAuth.ExecuteFirst())
	{
		QString CurrentPassword = qSelectAuth.ReadColumn("rolpassword").toString().remove(0, 3);
		QString InputCurrentPassword = ISSystem::StringFromMD5(EditCurrentPassword->GetValue().toString() + Login);

		if (CurrentPassword == InputCurrentPassword || !CurrentPassword.length()) //Сравнение MD5 хеша текущего пароля
		{
			ISQuery qAlterPassword;
			if (qAlterPassword.Execute(QA_PASSWORD.arg(Login).arg(EditPasswordCheck->GetValue().toString())))
			{
				if (UserID == CURRENT_USER_ID)
				{
					ISProtocol::Insert(true, CONST_UID_PROTOCOL_CHANGE_PASSWORD, QString(), QString(), QVariant());
				}

				ISQuery qInsertPasswordChange(QI_USER_PASSWORD_CHANGED);
				qInsertPasswordChange.BindValue(":User", UserID);
				qInsertPasswordChange.BindValue(":WhoUser", CURRENT_USER_ID);
				if (qInsertPasswordChange.Execute())
				{
					SetResult(true);
					close();
				}
			}
		}
		else
		{
			ISSystem::SetWaitGlobalCursor(false);
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotValidInputPasswordForChange"));
			EditCurrentPassword->BlinkRed();
		}
	}

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::PasswordChecked()
{
	QString Password = EditPassword->GetValue().toString();
	QString PasswordCheck = EditPasswordCheck->GetValue().toString();

	if (Password.length() && PasswordCheck.length())
	{
		if (EditPassword->GetValue().toString() == EditPasswordCheck->GetValue().toString())
		{
			ButtonDialog->SetApplyEnabled(true);
			SetIconAndTextHeader(BUFFER_ICONS("Apply.Green"), LOCALIZATION("PasswordsMatch"));
		}
		else
		{
			ButtonDialog->SetApplyEnabled(false);
			SetIconAndTextHeader(BUFFER_ICONS("Exit"), LOCALIZATION("PasswordsNotMatch"));
		}
	}
	else if (!Password.length() && !PasswordCheck.length())
	{
		ButtonDialog->SetApplyEnabled(false);
		SetIconAndTextHeader(BUFFER_ICONS("Keyboard"), LOCALIZATION("EnterThePassword"));
	}
	else
	{
		ButtonDialog->SetApplyEnabled(false);
		SetIconAndTextHeader(BUFFER_ICONS("Exit"), LOCALIZATION("OneFieldPasswordIsEmpty"));
	}
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::SetIconAndTextHeader(const QIcon &Icon, const QString &Text)
{
	LabelIcon->setPixmap(Icon.pixmap(SIZE_25_25));
	LabelText->setText(Text);
}
//-----------------------------------------------------------------------------
