#include "ISTaskObjectForm.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_STATUS = PREPARE_QUERY("SELECT tsst_id FROM _taskstatus WHERE tsst_uid = :StatusUID");
//-----------------------------------------------------------------------------
ISTaskObjectForm::ISTaskObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISTaskObjectForm::~ISTaskObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISTaskObjectForm::Save()
{
	ISQuery qSelectStatus(QS_STATUS);
	qSelectStatus.BindValue(":StatusUID", CONST_UID_TASK_STATUS_NEW);
	bool Result = qSelectStatus.ExecuteFirst();
	if (Result)
	{
		SetFieldValue("Status", qSelectStatus.ReadColumn("tsst_id"));
		Result = ISObjectFormBase::Save();
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.GettingStatusID"), qSelectStatus.GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
