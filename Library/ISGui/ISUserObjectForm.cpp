#include "ISUserObjectForm.h"
#include "ISConstants.h"
#include "ISControls.h"
#include "ISUserListForm.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISSystem.h"
#include "ISUserPhotoCreator.h"
//-----------------------------------------------------------------------------
static QString QC_USER = "CREATE ROLE \"%1\" SUPERUSER NOINHERIT NOREPLICATION LOGIN CONNECTION LIMIT 1";
//-----------------------------------------------------------------------------
static QString QA_LOGIN = "ALTER ROLE %1 RENAME TO %2";
//-----------------------------------------------------------------------------
static QString QS_LOGIN = PREPARE_QUERY("SELECT COUNT(*) FROM _users WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
static QString QU_OID = PREPARE_QUERY("UPDATE _users SET "
									  "usrs_oid = (SELECT usesysid FROM pg_user WHERE usename = :Login) "
									  "WHERE usrs_login = :Login");
//-----------------------------------------------------------------------------
ISUserObjectForm::ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	QAction *ActionChangePassword = ISControls::CreateActionPasswordChange(this);
	connect(ActionChangePassword, &QAction::triggered, this, &ISUserObjectForm::PasswordChange);
	AddActionToolBar(ActionChangePassword, true);

	QAction *ActionDeletePassword = ISControls::CreateActionPasswordDelete(this);
	connect(ActionDeletePassword, &QAction::triggered, this, &ISUserObjectForm::PasswordDelete);
	AddActionToolBar(ActionDeletePassword, true);

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
	CurrentLogin = EditLogin->GetValue().toString();
	if (GetFormType() == ISNamespace::OFT_Edit && EditAccountLifeTime->GetValue().toBool())
	{
		EditAccountLifeTimeStart->setEnabled(true);
		EditAccountLifeTimeEnd->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
bool ISUserObjectForm::Save()
{
	//Проверка привязки учётной записи к группе
	bool Result = GetFieldValue("Group").isValid();
	if (!Result)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.UserNotLinkedToGroup"));
		GetFieldWidget("Group")->BlinkRed();
		return Result;
	}

	//Если логин был изменен - проверяем новый на наличие
	if (EditLogin->GetValue().toString() != CurrentLogin)
	{
		ISQuery qSelectLogin(QS_LOGIN);
		qSelectLogin.BindValue(":Login", EditLogin->GetValue());
		Result = qSelectLogin.ExecuteFirst();
		if (Result)
		{
			Result = qSelectLogin.ReadColumn("count").toInt() == 0;
			if (!Result) //Если такой логин уже существует - выходим из функции
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Warning.LoginAlreadyExist").arg(EditLogin->GetValue().toString()));
				EditLogin->BlinkRed();
				return Result;
			}
		}
		else //Ошибка проверки наличия логина
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.CheckExistLogin").arg(EditLogin->GetValue().toString()), qSelectLogin.GetErrorString());
			return Result;
		}
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

	if (!GetFieldValue("Photo").isValid())
	{
		SetFieldValue("Photo", ISGui::PixmapToByteArray(ISUserPhotoCreator().Create(GetFieldValue("FIO").toString())));
	}

	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
	{
		Result = ISObjectFormBase::Save();
		if (Result)
		{
			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.CreatePasswordUser").arg(GetFieldValue("FIO").toString())))
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
	return Result;
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::SavedEvent()
{
	QString NewLogin = EditLogin->GetValue().toString();
	if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy) //Если происходит создание или создание копии пользователя
	{
		ISQuery qCreate;
		if (qCreate.Execute(QC_USER.arg(NewLogin)))
		{
			CurrentLogin = NewLogin;
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.CreateUserRole"), qCreate.GetErrorString());
		}
	}
	else if (GetFormType() == ISNamespace::OFT_Edit) //Если происходит редактирование пользователя
	{
		if (CurrentLogin != NewLogin) //Если логин пользователя изменился
		{
			ISQuery qAlterLogin;
			if (qAlterLogin.Execute(QA_LOGIN.arg(CurrentLogin).arg(NewLogin)))
			{
				CurrentLogin = NewLogin;
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.AlterUserRole"), qAlterLogin.GetErrorString());
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::SaveAfter()
{
	ISObjectFormBase::SaveAfter();
	ISQuery qUpdateOID(QU_OID);
	qUpdateOID.BindValue(":Login", CurrentLogin);
	if (!qUpdateOID.Execute())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.AlterOID"), qUpdateOID.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::PasswordChange()
{
	if (ISGui::ShowUserPasswordForm(GetObjectID()))
	{
		ISMessageBox::ShowInformation(this, LANG("Message.Information.ChangePasswordUser").arg(GetFieldValue("FIO").toString()));
	}
}
//-----------------------------------------------------------------------------
void ISUserObjectForm::PasswordDelete()
{
	//Если запись была изменена - просим сохранить
	GetModificationFlag() ? ISMessageBox::ShowWarning(this, LANG("Message.Warning.SaveObjectFromContinue")) : ISGui::ShowUserPasswordDelete(GetObjectID(), EditLogin->GetValue().toString());
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
