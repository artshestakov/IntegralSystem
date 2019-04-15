#include "StdAfx.h"
#include "ISCenterSevenPatientsObjectForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISCenterSevenPatientsObjectForm::ISCenterSevenPatientsObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	QAction *ActionWaitAdmission = new QAction(this);
	ActionWaitAdmission->setText(LOCALIZATION("CenterSeven.AddWaitAdmission"));
	ActionWaitAdmission->setToolTip(LOCALIZATION("CenterSeven.AddWaitAdmission"));
	ActionWaitAdmission->setIcon(BUFFER_ICONS("Wait"));
	connect(ActionWaitAdmission, &QAction::triggered, this, &ISCenterSevenPatientsObjectForm::AddWaitAdmission);
	AddActionToolBar(ActionWaitAdmission);
}
//-----------------------------------------------------------------------------
ISCenterSevenPatientsObjectForm::~ISCenterSevenPatientsObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenPatientsObjectForm::SetPhoneMain(const QString &PhoneMain)
{
	GetFieldWidget("PhoneMain")->SetValue(PhoneMain);
	GetFieldWidget("PhoneMain")->SetFocus();
}
//-----------------------------------------------------------------------------
void ISCenterSevenPatientsObjectForm::AddWaitAdmission()
{
	ISObjectFormBase *WaitAdmissionObjectForm = ISGui::CreateObjectForm(ISNamespace::OFT_New, "WaitAdmission");
	WaitAdmissionObjectForm->SetFieldValue("Patient", GetObjectID());
	WaitAdmissionObjectForm->show();
}
//-----------------------------------------------------------------------------
