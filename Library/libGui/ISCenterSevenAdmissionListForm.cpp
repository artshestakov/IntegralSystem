#include "ISCenterSevenAdmissionListForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISCenterSevenStatusAdmissionForm.h"
#include "ISGui.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QU_ADMISSION_STATUS = PREPARE_QUERY("UPDATE admission SET "
												   "adms_status = (SELECT adst_id FROM admissionstatus WHERE adst_uid = :StatusUID), "
												   "adms_statuscomment = :Comment "
												   "WHERE adms_id = :AdmissionID");
//-----------------------------------------------------------------------------
ISCenterSevenAdmissionListForm::ISCenterSevenAdmissionListForm(QWidget *parent) : ISListBaseForm("Admission", parent)
{
	QAction *ActionStatus = new QAction(this);
	ActionStatus->setText(LANG("CenterSeven.ChangeStatus"));
	ActionStatus->setToolTip(LANG("CenterSeven.ChangeStatus"));
	ActionStatus->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ActionStatus, &QAction::triggered, this, &ISCenterSevenAdmissionListForm::StatusChange);
	AddAction(ActionStatus, true, true);
}
//-----------------------------------------------------------------------------
ISCenterSevenAdmissionListForm::~ISCenterSevenAdmissionListForm()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionListForm::SetEnabledActions(bool Enabled)
{
	for (QAction *Action : GetToolBar()->actions())
	{
		Action->setEnabled(Enabled);
	}
}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionListForm::SetBranch(int branch_id)
{
	BranchID = branch_id;
}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionListForm::SetDate(const QDate &date)
{
	Date = date;
}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionListForm::SetDoctor(int doctor_id)
{
	DoctorID = doctor_id;
}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionListForm::Update()
{
	GetQueryModel()->ClearConditions();

	if (DoctorID)
	{
		GetQueryModel()->SetClassFilter("adms_branch = :BranchID AND adms_date = :Date AND adms_doctor = :DoctorID");
		GetQueryModel()->AddCondition(":DoctorID", DoctorID);
	}
	else
	{
		GetQueryModel()->SetClassFilter("adms_branch = :BranchID AND adms_date = :Date AND adms_doctor IN(SELECT dctr_id FROM doctor WHERE NOT dctr_isdeleted)");
	}

	
	GetQueryModel()->AddCondition(":BranchID", BranchID);
	GetQueryModel()->AddCondition(":Date", Date);
	
	ISListBaseForm::Update();
}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionListForm::StatusChange()
{
	ISGui::SetWaitGlobalCursor(true);
	int CurrentStatusID = ISDatabase::GetInstance().GetValue("Admission", "Status", GetObjectID()).toInt();
	ISUuid CurrentStatusUID = ISDatabase::GetInstance().GetValue("AdmissionStatus", "UID", CurrentStatusID);
	ISCenterSevenStatusAdmissionForm StatusAdmissionForm(CurrentStatusUID, GetCurrentRecordValue("StatusComment").toString());
	ISGui::SetWaitGlobalCursor(false);
	if (StatusAdmissionForm.Exec())
	{
		ISUuid SelectedStatusUID = StatusAdmissionForm.GetStatusUID();
		QString Comment = StatusAdmissionForm.GetComment();

		ISQuery qUpdateStatus(QU_ADMISSION_STATUS);
		qUpdateStatus.BindValue(":StatusUID", SelectedStatusUID);
		
		if (Comment.length())
		{
			qUpdateStatus.BindValue(":Comment", Comment);
		}
		else
		{
			qUpdateStatus.BindValue(":Comment", QVariant());
		}

		qUpdateStatus.BindValue(":AdmissionID", GetObjectID());
		if (qUpdateStatus.Execute())
		{
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
