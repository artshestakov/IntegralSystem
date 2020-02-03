#include "ISCenterSevenWaitAdmissionListForm.h"
#include "ISLocalization.h"
#include "ISObjects.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QU_CLOSE_WAIT = PREPARE_QUERY("UPDATE waitadmission SET "
											 "wadm_closed = true "
											 "WHERE wadm_id = :ObjectID");
//-----------------------------------------------------------------------------
ISCenterSevenWaitAdmissionListForm::ISCenterSevenWaitAdmissionListForm(QWidget *parent) : ISListBaseForm("WaitAdmission", parent)
{
	QAction *ActionClose = new QAction(this);
	ActionClose->setText(LANG("CenterSeven.CloseWait"));
	ActionClose->setToolTip(LANG("CenterSeven.CloseWait"));
	ActionClose->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("CloseWait"));
	connect(ActionClose, &QAction::triggered, this, &ISCenterSevenWaitAdmissionListForm::CloseWait);
	AddAction(ActionClose, true, true);
}
//-----------------------------------------------------------------------------
ISCenterSevenWaitAdmissionListForm::~ISCenterSevenWaitAdmissionListForm()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenWaitAdmissionListForm::CloseWait()
{
	if (GetCurrentRecordValue("Closed").toBool())
	{
		ISMessageBox::ShowWarning(this, LANG("CenterSeven.Message.Warning.AlreadyCloseWait"));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("CenterSeven.Message.Question.CloseWait")))
	{
		ISQuery qCloseWait(QU_CLOSE_WAIT);
		qCloseWait.BindValue(":ObjectID", GetObjectID());
		if (qCloseWait.Execute())
		{
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
