#include "ISUserObjectForm.h"
#include "ISConstants.h"
#include "ISControls.h"
#include "ISUserListForm.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISNotifySender.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QC_USER = "CREATE ROLE \"%1\" SUPERUSER NOINHERIT NOREPLICATION LOGIN CONNECTION LIMIT 1";
//-----------------------------------------------------------------------------
static QString QA_LOGIN = "ALTER ROLE %1 RENAME TO %2";
//-----------------------------------------------------------------------------
ISUserObjectForm::ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	QAction *ActionChangePassword = ISControls::CreateActionPasswordChange(this);
	connect(ActionChangePassword, &QAction::triggered, this, &ISUserObjectForm::PasswordChange);
	AddActionToolBar(ActionChangePassword, true);

	EditLogin = GetFieldWidget("Login");

	EditAccountLifeTime = GetFieldWidget("AccountLifeTime");
	connect(EditAccountLifeTime, &ISFieldEditBase::DataChanged, this, &ISUserObjectForm::AccountLifeTimeChanged);

	EditAccountLifeTimeStart = GetFieldWidget("AccountLifeTimeStart");
	EditAccountLifeTimeStart->setEnabled(false);

	EditAccountLifeTimeEnd = GetFieldWidget("AccountLifeTimeEnd");
	EditAccountLifeTimeEnd->setEnabled(false);
}
//-----------------------------------------------------------------------------
ISUserObjectForm::~ISUserObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISUserObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	LoginOld = EditLogin->GetValue().toString();
	
	if (GetFormType() == ISNamespace::OFT_Edit && EditAccountLifeTime->GetValue().toBool())
	{
		EditAccountLifeTimeStart->setEnabled(true);
		EditAccountLifeTimeEnd->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
bool ISUserObjectForm::Save()
{
	bool Result = true;

	//Проверка привязки учётной записи к группе
	if (!GetFieldValue("Group").isValid())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.UserNotLinkedToGroup"));
		GetFieldWidget("Group")->BlinkRed();
	}

	//Проверка корректности ввода диапазона срока действия учётной записи
	if (EditAccountLifeTime->GetValue().toBool())
	{
		QDate DateStart = EditAccountLifeTimeStart->GetValue().toDate();
		QDate DateEnd = EditAccountLifeTimeEnd->GetValue().toDate();
		if (DateStart > DateEnd || DateEnd < DateStart || !DateStart.isValid() || !DateEnd.isValid())
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.InvalidAccountLifeTimeDatesUser"));
			EditAccountLifeTimeStart->BlinkRed();
			EditAccountLifeTimeEnd->BlinkRed();
		}
	}

	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
	{
		Result = ISObjectFormBase::Save();
		if (Result)
		{
			QString UserFullName = GetFieldValue("Surname").toString() + SYMBOL_SPACE + GetFieldValue("Name").toString() + SYMBOL_SPACE + GetFieldValue("Patronymic").toString();
			ISSystem::RemoveLastSymbolLoop(UserFullName, SYMBOL_SPACE);
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CreatePasswordUser").arg(UserFullName)))
			{
				if (ISGui::ShowUserPasswordForm(GetObjectID()))
				{
					ISMessageBox::ShowInformation(this, LANG("Message.Information.CreatedPasswordUser"));
				}
			}
			else
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotCreatedUserPassword"));
			}
		}
	}
	else
	{
		Result = ISObjectFormBase::Save();
	}

	if (Result)
	{
		ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_USER_CHANGED, GetObjectID(), QVariant(), QString(), false);
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::SavedEvent()
{
	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy) //Если происходит создание или создание копии пользователя
	{
		ISQuery qCreate;
		if (qCreate.Execute(QC_USER.arg(EditLogin->GetValue().toString())))
		{
			LoginOld = EditLogin->GetValue().toString();
		}
	}
	else if (GetFormType() == ISNamespace::OFT_Edit) //Если происходит редактирование пользователя
	{
		if (LoginOld != EditLogin->GetValue().toString()) //Если логин пользователя изменился
		{
			ISQuery qAlterLogin;
			if (qAlterLogin.Execute(QA_LOGIN.arg(LoginOld).arg(EditLogin->GetValue().toString())))
			{
				LoginOld = EditLogin->GetValue().toString();
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::PasswordChange()
{
	if (ISGui::ShowUserPasswordForm(GetObjectID()))
	{
		QString FullName = GetFieldValue("Surname").toString() + SYMBOL_SPACE + GetFieldValue("Name").toString() + SYMBOL_SPACE + GetFieldValue("Patronymic").toString();
		ISMessageBox::ShowInformation(this, LANG("Message.Information.ChangePasswordUser").arg(FullName));
	}
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::AccountLifeTimeChanged()
{
	bool Enabled = EditAccountLifeTime->GetValue().toBool();
	EditAccountLifeTimeStart->setEnabled(Enabled);
	EditAccountLifeTimeEnd->setEnabled(Enabled);
	if (!Enabled)
	{
		EditAccountLifeTimeStart->Clear();
		EditAccountLifeTimeEnd->Clear();
	}
}
//-----------------------------------------------------------------------------
