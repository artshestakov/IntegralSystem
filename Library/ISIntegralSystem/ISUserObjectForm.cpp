#include "StdAfx.h"
#include "ISUserObjectForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISControls.h"
#include "ISUserListForm.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISNotifySender.h"
//-----------------------------------------------------------------------------
static QString QC_USER = "CREATE ROLE \"%1\" SUPERUSER NOINHERIT NOREPLICATION LOGIN CONNECTION LIMIT 1";
//-----------------------------------------------------------------------------
static QString QA_LOGIN = "ALTER ROLE %1 RENAME TO %2";
//-----------------------------------------------------------------------------
ISUserObjectForm::ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
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

	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
	{
		Result = ISObjectFormBase::Save();
		if (Result)
		{
			QString UserFullName = GetFieldValue("Surname").toString() + SYMBOL_SPACE + GetFieldValue("Name").toString() + SYMBOL_SPACE + GetFieldValue("Patronymic").toString();
			if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.CreatePasswordUser").arg(UserFullName)))
			{
				if (ISGui::ShowUserPasswordForm(GetObjectID()))
				{
					ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.CreatedPasswordUser"));
				}
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
void ISUserObjectForm::SaveEvent()
{
	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy) //≈сли происходит создание или создание копии пользовател€
	{
		ISQuery qCreate;
		if (qCreate.Execute(QC_USER.arg(EditLogin->GetValue().toString())))
		{
			LoginOld = EditLogin->GetValue().toString();
		}
	}
	else if (GetFormType() == ISNamespace::OFT_Edit) //≈сли происходит редактирование пользовател€
	{
		if (LoginOld != EditLogin->GetValue().toString()) //≈сли логин пользовател€ изменилс€
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
		ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.ChangePasswordUser").arg(FullName));
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
