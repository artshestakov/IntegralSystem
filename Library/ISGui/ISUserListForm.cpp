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

	QAction *ActionDeletePassword = ISControls::CreateActionPasswordDelete(this);
	connect(ActionDeletePassword, &QAction::triggered, this, &ISUserListForm::DeletePassword);
	AddAction(ActionDeletePassword, true, true);
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
void ISUserListForm::DeletePassword()
{
	ISDatabase::Instance().GetValue("_Users", "IsSystem", GetObjectID()).toBool() ?
		ISMessageBox::ShowWarning(this, LANG("Message.User.ChangePassword.Postgres")) :
		ISGui::ShowUserPasswordDelete(GetCurrentRecordValue("Login").toString());
}
//-----------------------------------------------------------------------------
bool ISUserListForm::CheckThisUser()
{
	return GetObjectID() == CURRENT_USER_ID;
}
//-----------------------------------------------------------------------------
