#include "ISCenterSevenAdmissionObjectForm.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISProperty.h"
#include "ISListEdit.h"
//-----------------------------------------------------------------------------
static QString QS_ADMISSION_STATUS = PREPARE_QUERY2("SELECT adst_id "
												   "FROM admissionstatus "
												   "WHERE adst_uid = :StatusUID");
//-----------------------------------------------------------------------------
ISCenterSevenAdmissionObjectForm::ISCenterSevenAdmissionObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	ISQuery qSelectStatus(QS_ADMISSION_STATUS);
	qSelectStatus.BindValue(":StatusUID", "{92BF6D81-5A6C-4333-8783-5436BCD41A64}");
	qSelectStatus.ExecuteFirst();
	StatusID = qSelectStatus.ReadColumn("adst_id").toInt();

	ISListEdit *ListEdit = dynamic_cast<ISListEdit*>(GetFieldWidget("Place"));
	ListEdit->SetSqlFilter("arch_Branch = " + ISProperty::Instance().GetValue("BranchID").toString());
}
//-----------------------------------------------------------------------------
ISCenterSevenAdmissionObjectForm::~ISCenterSevenAdmissionObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionObjectForm::SetDate(const QDate &date)
{
	Date = date;
}
//-----------------------------------------------------------------------------
void ISCenterSevenAdmissionObjectForm::SetDoctor(int doctor_id)
{
	GetFieldWidget("Doctor")->SetValue(doctor_id);
}
//-----------------------------------------------------------------------------
bool ISCenterSevenAdmissionObjectForm::Save()
{
	bool Result = false;
	QTime SelectedTime = GetFieldValue("Time").toTime();
	if (SelectedTime >= QTime(9, 0) && SelectedTime <= QTime(20, 0))
	{
		if (GetFormType() == ISNamespace::OFT_New || GetFormType() == ISNamespace::OFT_Copy)
		{
			GetFieldWidget("Branch")->SetValue(ISProperty::Instance().GetValue("BranchID"));
			GetFieldWidget("Date")->SetValue(Date);
			GetFieldWidget("Status")->SetValue(StatusID);
		}

		Result = ISObjectFormBase::Save();
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("CenterSeven.Message.Warning.AdmissionTimeInvalid"));
		GetFieldWidget("Time")->BlinkRed();
		Result = false;
	}

	return Result;
}
//-----------------------------------------------------------------------------
