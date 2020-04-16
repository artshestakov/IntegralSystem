#include "ISCenterSevenIncomingCallForm.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISCenterSevenPatientsObjectForm.h"
//-----------------------------------------------------------------------------
static QString QS_PATIENTS = PREPARE_QUERY2("SELECT ptnt_fio FROM patients WHERE ptnt_id = :PatientID");
//-----------------------------------------------------------------------------
ISCenterSevenIncomingCallForm::ISCenterSevenIncomingCallForm(const QVariantMap &payload) : ISIncomingCallBaseForm(payload)
{
	QStringList StringList = payload.value("Payload").toString().split('_');
	PatientID = StringList.first().toInt();
	PhoneMain = StringList.last();

	ISQuery qSelect(QS_PATIENTS);
	qSelect.BindValue(":PatientID", PatientID);
	if (qSelect.ExecuteFirst())
	{
		AddLabel(LANG("CenterSeven.CallingPatient") + ':', qSelect.ReadColumn("ptnt_fio").toString());
	}
	else
	{
		AddLabel(LANG("CenterSeven.CallingPatient") + ':', LANG("CenterSeven.CallingPatient.Unknown").arg(PhoneMain));
	}
}
//-----------------------------------------------------------------------------
ISCenterSevenIncomingCallForm::~ISCenterSevenIncomingCallForm()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenIncomingCallForm::OpenCard()
{
	ISCenterSevenPatientsObjectForm *PatinetsObjectForm = nullptr;
	if (PatientID)
	{
		PatinetsObjectForm = dynamic_cast<ISCenterSevenPatientsObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Patients", PatientID));
	}
	else
	{
		PatinetsObjectForm = dynamic_cast<ISCenterSevenPatientsObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "Patients"));
		PatinetsObjectForm->SetPhoneMain(PhoneMain);
	}

	PatinetsObjectForm->show();
	close();
}
//-----------------------------------------------------------------------------
