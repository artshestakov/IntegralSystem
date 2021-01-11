#include "ISTaskObjectForm.h"
#include "ISUserRoleEntity.h"
#include "ISMessageBox.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISSettings.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISTaskObjectForm::ISTaskObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	//≈сли нет прав на назначение исполнител€
	if (!ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_APPOINT))
	{
		GetFieldWidget("Executor")->SetReadOnly(true);
		GetFieldWidget("Executor")->SetToolTip(LANG("Message.Warning.NotAccess.Special.Appoint"));
		GetFieldWidget("Executor")->SetCursor(CURSOR_WHATS_THIS);
	}

	CheckEdit = new ISCheckEdit(GetToolBar());
	CheckEdit->SetValue(SETTING_BOOL(CONST_UID_SETTING_TASK_TRACKER_SHOW_TASK_AFTER_CREATE));
	CheckEdit->SetText(LANG("Task.Setting.ShowTaskViewFormAfterCreate"));
	connect(CheckEdit, &ISCheckEdit::DataChanged, this, &ISTaskObjectForm::CheckEditClicked);
	GetToolBar()->addWidget(CheckEdit);
}
//-----------------------------------------------------------------------------
ISTaskObjectForm::~ISTaskObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISTaskObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result && CheckEdit->GetValue().toBool())
	{
		ISGui::ShowTaskViewForm(GetObjectID());
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTaskObjectForm::CheckEditClicked()
{
	ISSettings::Instance().SetValue(CONST_UID_SETTING_TASK_TRACKER_SHOW_TASK_AFTER_CREATE, CheckEdit->GetValue());
}
//-----------------------------------------------------------------------------
