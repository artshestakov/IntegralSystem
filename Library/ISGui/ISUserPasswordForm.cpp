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
#include "ISPasswordWidthWidget.h"
//-----------------------------------------------------------------------------
static QString QS_USER = PREPARE_QUERY("SELECT usrs_login, userfullname(:UserID), CASE WHEN length(passwd) > 0 THEN true ELSE false END "
									   "FROM _users "
									   "LEFT JOIN pg_shadow ON usename = usrs_login "
									   "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_AUTHID = PREPARE_QUERY("SELECT rolpassword "
										 "FROM pg_authid "
										 "WHERE rolname = :Login");
//-----------------------------------------------------------------------------
static QString QA_PASSWORD = "ALTER ROLE %1 WITH ENCRYPTED PASSWORD '%2'";
//-----------------------------------------------------------------------------
static QString QI_USER_PASSWORD_CHANGED = PREPARE_QUERY("INSERT INTO _userpasswordchanged(upcg_user, upcg_type, upcg_whouser) "
														"VALUES(:User, (SELECT upct_id FROM _userpasswordchangedtype WHERE upct_uid = :ChangeTypeUID), :WhoUser)");
//-----------------------------------------------------------------------------
ISUserPasswordForm::ISUserPasswordForm(int user_id) : ISInterfaceDialogForm()
{
	ISQuery qSelectUser(QS_USER);
	qSelectUser.BindValue(":UserID", user_id);
	if (qSelectUser.ExecuteFirst())
	{
		UserID = user_id;
		Login = qSelectUser.ReadColumn("usrs_login").toString();
		UserFullName = qSelectUser.ReadColumn("userfullname").toString();
		ExistPassword = qSelectUser.ReadColumn("case").toBool();
	}

	setWindowTitle(ExistPassword ? LANG("ChangingPassword") : LANG("CreatingPassword"));
	ForbidResize();
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	GetMainLayout()->addWidget(new QLabel(LANG("User") + ": " + UserFullName, this));

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS("Keyboard").pixmap(ISDefines::Gui::SIZE_25_25));
	Layout->addWidget(LabelIcon);

	LabelText = new QLabel(LANG("EnterThePassword"), this);
	Layout->addWidget(LabelText);

	Layout->addStretch();

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	if (ExistPassword)
	{
		EditCurrentPassword = new ISLineEdit(this);
		EditCurrentPassword->SetEchoMode(QLineEdit::Password);
		FormLayout->addRow(LANG("CurrentPassword") + ':', EditCurrentPassword);
	}

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
	ExistPassword ? EditCurrentPassword->SetFocus() : EditPassword->SetFocus();
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::EnterClicked()
{
	ChangePassword();
}
//-----------------------------------------------------------------------------
void ISUserPasswordForm::ChangePassword()
{
	if (ExistPassword) //Если пароль есть - сравниваем с тем что ввели
	{
		if (!EditCurrentPassword->GetValue().isValid()) //Если текущий пароль не ввели - выходим из функции
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("CurrentPassword")));
			EditCurrentPassword->BlinkRed();
			return;
		}

		ISQuery qSelectAuth(QS_AUTHID);
		qSelectAuth.BindValue(":Login", Login);
		if (qSelectAuth.ExecuteFirst()) //Если запрос на текущий пароль прошел успешно - проверяем с тем что ввели
		{
			if (qSelectAuth.ReadColumn("rolpassword").toString().remove(0, 3) != ISSystem::StringToMD5(EditCurrentPassword->GetValue().toString() + Login))
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotValidInputPasswordForChange"));
				EditCurrentPassword->BlinkRed();
				return;
			}
		}
		else //Не удалось запросить текущий пароль
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectCurrentPasswordUser"), qSelectAuth.GetErrorString());
			return;
		}
	}

	if (!EditPassword->GetValue().isValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("Password")));
		EditPassword->BlinkRed();
		return;
	}

	if (EditPassword->GetValue().toString().length() < MINIMUM_PASSWORD_LENGHT)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.PasswordLenghtSmall").arg(MINIMUM_PASSWORD_LENGHT));
		EditPassword->BlinkRed();
		return;
	}

	if (!EditPasswordCheck->GetValue().isValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.Field.NullValue").arg(LANG("PasswordCheck")));
		EditPasswordCheck->BlinkRed();
		return;
	}

	ISQuery qAlterPassword;
	if (qAlterPassword.Execute(QA_PASSWORD.arg(Login).arg(EditPasswordCheck->GetValue().toString())))
	{
		if (UserID == CURRENT_USER_ID)
		{
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_CHANGE_PASSWORD, QString(), QString(), QVariant());
		}

		ISQuery qInsertPasswordChange(QI_USER_PASSWORD_CHANGED);
		qInsertPasswordChange.BindValue(":User", UserID);
		qInsertPasswordChange.BindValue(":ChangeTypeUID", ExistPassword ? CONST_UID_USER_PASSWORD_CHANGE_TYPE_UPDATE : CONST_UID_USER_PASSWORD_CHANGE_TYPE_CREATE);
		qInsertPasswordChange.BindValue(":WhoUser", CURRENT_USER_ID);
		if (!qInsertPasswordChange.Execute())
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.ChangePasswordUserHistory"), qInsertPasswordChange.GetErrorString());
		}
		SetResult(true);
		close();
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.ChangePasswordUser"), qAlterPassword.GetErrorString());
	}
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
	LabelIcon->setPixmap(Icon.pixmap(ISDefines::Gui::SIZE_25_25));
	LabelText->setText(Text);
}
//-----------------------------------------------------------------------------
