#include "ISUserPasswordForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISSystem.h"
#include "ISProtocol.h"
#include "ISGui.h"
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

	setWindowTitle(LANG("ChangePassword"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_10_PX);

	GetMainLayout()->addWidget(new QLabel(LANG("User") + ": " + UserFullName, this));

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS("Keyboard").pixmap(DEFINES_GUI.SIZE_25_25));
	Layout->addWidget(LabelIcon);

	LabelText = new QLabel(LANG("EnterThePassword"), this);
	Layout->addWidget(LabelText);

	Layout->addStretch();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	EditCurrentPassword = new ISLineEdit(this);
	EditCurrentPassword->SetEchoMode(QLineEdit::Password);
	FormLayout->addRow(LANG("CurrentPassword") + ':', EditCurrentPassword);

	EditPassword = new ISLineEdit(this);
	EditPassword->SetEchoMode(QLineEdit::Password);
	connect(EditPassword, &ISLineEdit::DataChanged, this, &ISUserPasswordForm::PasswordChecked);
	FormLayout->addRow(LANG("Password") + ':', EditPassword);

	EditPasswordCheck = new ISLineEdit(this);
	EditPasswordCheck->SetEchoMode(QLineEdit::Password);
	connect(EditPasswordCheck, &ISLineEdit::DataChanged, this, &ISUserPasswordForm::PasswordChecked);
	FormLayout->addRow(LANG("PasswordCheck") + ':', EditPasswordCheck);

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
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.PasswordLenghtSmall").arg(MINIMUM_PASSWORD_LENGHT));
		return;
	}

	ISGui::SetWaitGlobalCursor(true);

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
				if (UserID == ISMetaUser::GetInstance().GetData()->ID)
				{
					ISProtocol::Insert(true, CONST_UID_PROTOCOL_CHANGE_PASSWORD, QString(), QString(), QVariant());
				}

				ISQuery qInsertPasswordChange(QI_USER_PASSWORD_CHANGED);
				qInsertPasswordChange.BindValue(":User", UserID);
				qInsertPasswordChange.BindValue(":WhoUser", ISMetaUser::GetInstance().GetData()->ID);
				if (qInsertPasswordChange.Execute())
				{
					SetResult(true);
					close();
				}
			}
		}
		else
		{
			ISGui::SetWaitGlobalCursor(false);
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotValidInputPasswordForChange"));
			EditCurrentPassword->BlinkRed();
		}
	}

	ISGui::SetWaitGlobalCursor(false);
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
			SetIconAndTextHeader(BUFFER_ICONS("Apply.Green"), LANG("PasswordsMatch"));
		}
		else
		{
			ButtonDialog->SetApplyEnabled(false);
			SetIconAndTextHeader(BUFFER_ICONS("Exit"), LANG("PasswordsNotMatch"));
		}
	}
	else if (!Password.length() && !PasswordCheck.length())
	{
		ButtonDialog->SetApplyEnabled(false);
		SetIconAndTextHeader(BUFFER_ICONS("Keyboard"), LANG("EnterThePassword"));
	}
	else
	{
		ButtonDialog->SetApplyEnabled(false);
		SetIconAndTextHeader(BUFFER_ICONS("Exit"), LANG("OneFieldPasswordIsEmpty"));
	}
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::SetIconAndTextHeader(const QIcon &Icon, const QString &Text)
{
	LabelIcon->setPixmap(Icon.pixmap(DEFINES_GUI.SIZE_25_25));
	LabelText->setText(Text);
}
//-----------------------------------------------------------------------------
