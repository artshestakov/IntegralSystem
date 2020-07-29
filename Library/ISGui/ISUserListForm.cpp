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
	QAction *ActionChangePassword = ISControls::CreateActionPasswordChange(this);
	connect(ActionChangePassword, &QAction::triggered, this, &ISUserListForm::ChangePassword);
	AddAction(ActionChangePassword, true, true);
}
//-----------------------------------------------------------------------------
ISUserListForm::~ISUserListForm()
{

}
//-----------------------------------------------------------------------------
void ISUserListForm::CreateCopy()
{
	if (CheckThisUser())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotCreateCopyThisUser"));
	}
	else
	{
		ISListBaseForm::CreateCopy();
	}
}
//-----------------------------------------------------------------------------
void ISUserListForm::Edit()
{
	if (CheckThisUser())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotEditThisUser"));
	}
	else
	{
		ISListBaseForm::Edit();
	}
}
//-----------------------------------------------------------------------------
void ISUserListForm::ChangePassword()
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

	if (ISGui::ShowUserPasswordForm(GetCurrentRecordValue("ID").toInt()))
	{
		QString FullName = GetCurrentRecordValue("Surname").toString() + SYMBOL_SPACE + GetCurrentRecordValue("Name").toString() + SYMBOL_SPACE + GetCurrentRecordValue("Patronymic").toString();
		ISMessageBox::ShowInformation(this, LANG("Message.Information.ChangePasswordUser").arg(FullName));
	}
}
//-----------------------------------------------------------------------------
bool ISUserListForm::CheckThisUser()
{
	if (GetObjectID() == CURRENT_USER_ID)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
