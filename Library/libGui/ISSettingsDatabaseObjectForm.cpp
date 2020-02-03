#include "ISSettingsDatabaseObjectForm.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISNotifySender.h"
#include "ISMessageBox.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
ISSettingsDatabaseObjectForm::ISSettingsDatabaseObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id)
	: ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISSettingsDatabaseObjectForm::~ISSettingsDatabaseObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISSettingsDatabaseObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_SETTINGS_DATABASE_CHANGED, QVariant(), QString(), false);
		if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.SettingsDatabaseSaved")))
		{
			ISCore::ChangeUser();
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
