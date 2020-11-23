#include "ISUserListForm.h"
#include "ISConstants.h"
#include "ISControls.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISMetaUser.h"
#include "ISGui.h"
#include "ISQuery.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
ISUserListForm::ISUserListForm(QWidget *parent) : ISListBaseForm("_Users", parent)
{
	QAction *ActionPassword = ISControls::CreateActionUserPassword(this);
	connect(ActionPassword, &QAction::triggered, this, &ISUserListForm::PasswordManagement);
	AddAction(ActionPassword, true, true);
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
bool ISUserListForm::CheckThisUser()
{
	return (unsigned int)GetObjectID() == CURRENT_USER_ID;
}
//-----------------------------------------------------------------------------
