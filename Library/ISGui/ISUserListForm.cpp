#include "ISUserListForm.h"
#include "ISConstants.h"
#include "ISControls.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaUser.h"
#include "ISGui.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISTcpQuery.h"
#include "ISPopupMessage.h"
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
ISUserListForm::ISUserListForm(QWidget *parent) : ISListBaseForm("_Users", parent)
{
	QAction *ActionPassword = ISControls::CreateActionUserPassword(this);
	connect(ActionPassword, &QAction::triggered, this, &ISUserListForm::PasswordManagement);
	AddAction(ActionPassword, true, true);

	QAction *ActionPasswordReset = ISControls::CreateActionUserPasswordReset(this);
	connect(ActionPasswordReset, &QAction::triggered, this, &ISUserListForm::PasswordReset);
	AddAction(ActionPasswordReset);
}
//-----------------------------------------------------------------------------
ISUserListForm::~ISUserListForm()
{

}
//-----------------------------------------------------------------------------
void ISUserListForm::CreateCopy()
{
	CheckThisUser() ? ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotCreateCopyThisUser")) : ISListBaseForm::CreateCopy();
}
//-----------------------------------------------------------------------------
void ISUserListForm::Edit()
{
	CheckThisUser() ? ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotEditThisUser")) : ISListBaseForm::Edit();
}
//-----------------------------------------------------------------------------
void ISUserListForm::PasswordManagement()
{
	if (CheckThisUser())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotEditThisUser"));
		return;
	}

	if (ISDatabase::Instance().GetValue("_Users", "IsSystem", GetObjectID()).toBool())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.User.ChangePassword.Postgres"));
		return;
	}
	ISGui::ShowUserPasswordForm(GetCurrentRecordValue("ID").toUInt(), GetCurrentRecordValue("FIO").toString(), GetCurrentRecordValue("Login").toString());
}
//-----------------------------------------------------------------------------
void ISUserListForm::PasswordReset()
{
	if (!ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_USER_PASSWORD_RESET))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccessPasswordReset"));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.PasswordReset")))
	{
		ISTcpQuery qPasswordReset(API_USER_PASSWORD_RESET);
		qPasswordReset.BindValue("UserID", GetObjectID());
		if (qPasswordReset.Execute())
		{
			ISPopupMessage::ShowNotification(LANG("PasswordResetDone"));
		}
		else
		{
			ISMessageBox::ShowCritical(this, qPasswordReset.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
bool ISUserListForm::CheckThisUser()
{
	return (unsigned int)GetObjectID() == CURRENT_USER_ID;
}
//-----------------------------------------------------------------------------
