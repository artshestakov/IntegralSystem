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
static QString QS_USERS_ONLINE = PREPARE_QUERY("SELECT COUNT(*) "
											   "FROM _users "
											   "WHERE useronline(usrs_login)");
//-----------------------------------------------------------------------------
ISUserListForm::ISUserListForm(QWidget *parent) : ISListBaseForm("_Users", parent)
{
	QAction *ActionChangePassword = ISControls::CreateActionPasswordChange(this);
	connect(ActionChangePassword, &QAction::triggered, this, &ISUserListForm::ChangePassword);
	AddAction(ActionChangePassword, true, true);

	LabelOnline = new QLabel(this);
	GetToolBar()->addWidget(LabelOnline);
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
void ISUserListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();
	UpdateLabelOnline();
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
void ISUserListForm::UpdateLabelOnline()
{
	ISQuery qSelect(QS_USERS_ONLINE);
	if (qSelect.ExecuteFirst())
	{
		LabelOnline->setText(LANG("OnlineUsers") + ": " + qSelect.ReadColumn("count").toString());
	}
}
//-----------------------------------------------------------------------------
bool ISUserListForm::CheckThisUser()
{
	if (GetObjectID() == ISMetaUser::GetInstance().GetData()->ID)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
