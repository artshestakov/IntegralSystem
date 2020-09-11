#include "ISTaskObjectForm.h"
#include "ISUserRoleEntity.h"
#include "ISMessageBox.h"
#include "ISConstants.h"
#include "ISLocalization.h"
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
}
//-----------------------------------------------------------------------------
ISTaskObjectForm::~ISTaskObjectForm()
{

}
//-----------------------------------------------------------------------------
