#include "ISCenterSevenPatientsListForm.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISProgressForm.h"
#include "ISCore.h"
#include "ISObjects.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_PATIENT = PREPARE_QUERY2("SELECT ptnt_phonemain, ptnt_email "
										  "FROM patients "
										  "WHERE ptnt_id = :PatientID "
										  "AND NOT ptnt_refusemailings");
//-----------------------------------------------------------------------------
ISCenterSevenPatientsListForm::ISCenterSevenPatientsListForm(QWidget *parent) : ISListBaseForm("Patients", parent)
{
	QAction *ActionWaitAdmission = new QAction(this);
	ActionWaitAdmission->setText(LANG("CenterSeven.AddWaitAdmission"));
	ActionWaitAdmission->setToolTip(LANG("CenterSeven.AddWaitAdmission"));
	ActionWaitAdmission->setIcon(BUFFER_ICONS("Wait"));
	connect(ActionWaitAdmission, &QAction::triggered, this, &ISCenterSevenPatientsListForm::AddWaitAdmission);
	AddAction(ActionWaitAdmission, true, true);
}
//-----------------------------------------------------------------------------
ISCenterSevenPatientsListForm::~ISCenterSevenPatientsListForm()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenPatientsListForm::AddWaitAdmission()
{
	ISObjectFormBase *WaitAdmissionObjectForm = ISGui::CreateObjectForm(ISNamespace::OFT_New, "WaitAdmission");
	WaitAdmissionObjectForm->SetFieldValue("Patient", GetObjectID());
	WaitAdmissionObjectForm->show();
}
//-----------------------------------------------------------------------------
