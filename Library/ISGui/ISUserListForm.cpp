#include "ISUserListForm.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
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
	QAction *ActionPassword = new QAction(BUFFER_ICONS("User.Password"), LANG("PasswordManagement"), this);
	connect(ActionPassword, &QAction::triggered, this, &ISUserListForm::PasswordManagement);
	AddAction(ActionPassword, true, true);

	QAction *ActionPasswordReset = new QAction(BUFFER_ICONS("User.Password.Reset"), LANG("PasswordReset"), this);
	connect(ActionPasswordReset, &QAction::triggered, this, &ISUserListForm::PasswordReset);
	AddAction(ActionPasswordReset);
}
//-----------------------------------------------------------------------------
ISUserListForm::~ISUserListForm()
{

}
//-----------------------------------------------------------------------------
void ISUserListForm::PasswordManagement()
{
	if (GetCurrentRecordValue("Login").toString() == SYSTEM_USER_LOGIN)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.ManagementPasswordSystemUser"));
		return;
	}
	ISGui::ShowUserPasswordForm(GetCurrentRecordValue("ID").toUInt(), GetCurrentRecordValue("FIO").toString(), GetCurrentRecordValue("Login").toString());
}
//-----------------------------------------------------------------------------
void ISUserListForm::PasswordReset()
{
	if (GetCurrentRecordValue("Login").toString() == SYSTEM_USER_LOGIN)
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.ResetPasswordSystemUser"));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.PasswordReset"), LANG("ThisActionIsNotReversible")))
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
