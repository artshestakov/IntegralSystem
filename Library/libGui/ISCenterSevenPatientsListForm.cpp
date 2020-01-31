#include "StdAfx.h"
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
static QString QS_PATIENT = PREPARE_QUERY("SELECT ptnt_phonemain, ptnt_email "
										  "FROM patients "
										  "WHERE ptnt_id = :PatientID "
										  "AND NOT ptnt_refusemailings");
//-----------------------------------------------------------------------------
static QString QS_TEMPLATE = PREPARE_QUERY("SELECT stmp_text FROM smstemplate WHERE stmp_id = :TemplateID");
//-----------------------------------------------------------------------------
ISCenterSevenPatientsListForm::ISCenterSevenPatientsListForm(QWidget *parent) : ISListBaseForm("Patients", parent)
{
	QAction *ActionBirthday = new QAction(this);
	ActionBirthday->setText(LANG("Send"));
	ActionBirthday->setToolTip(LANG("Send"));
	ActionBirthday->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("SendMessage"));
	connect(ActionBirthday, &QAction::triggered, this, &ISCenterSevenPatientsListForm::Send);
	AddAction(ActionBirthday, false);

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
void ISCenterSevenPatientsListForm::Send()
{
	if (ISMessageBox::ShowQuestion(this, LANG("CenterSeven.Message.Question.SendMessagePatients").arg(GetSqlModel()->rowCount())))
	{
		int TemplateID = ISGui::SelectObject("SMSTemplate");
		if (TemplateID)
		{
			QString TemplateText = GetSMSTemplate(TemplateID);
			QVectorInt VectorInt = GetIDs();

			ISProgressForm ProgressForm(0, VectorInt.count(), this);
			ProgressForm.show();
			int Iterator = 1;

			for (int PatientID : VectorInt)
			{
				ProgressForm.SetText(LANG("CenterSeven.Sending").arg(Iterator).arg(VectorInt.count()));
				ProgressForm.AddOneValue();

				ISQuery qSelect(QS_PATIENT);
				qSelect.BindValue(":PatientID", PatientID);
				if (qSelect.ExecuteFirst())
				{
					QString Phone = qSelect.ReadColumn("ptnt_phonemain").toString();
					if (Phone.length())
					{
						ISCore::SMSSend("7" + Phone, TemplateText);
					}

					QString PatientEMail = qSelect.ReadColumn("ptnt_email").toString();
					if (PatientEMail.length())
					{
						ISCore::EMailSend(PatientEMail, "Birthday subject", TemplateText);
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISCenterSevenPatientsListForm::AddWaitAdmission()
{
	ISObjectFormBase *WaitAdmissionObjectForm = ISGui::CreateObjectForm(ISNamespace::OFT_New, "WaitAdmission");
	WaitAdmissionObjectForm->SetFieldValue("Patient", GetObjectID());
	WaitAdmissionObjectForm->show();
}
//-----------------------------------------------------------------------------
QString ISCenterSevenPatientsListForm::GetSMSTemplate(int TemplateID) const
{
	QString Text;
	ISQuery qSelect(QS_TEMPLATE);
	qSelect.BindValue(":TemplateID", TemplateID);
	if (qSelect.ExecuteFirst())
	{
		Text = qSelect.ReadColumn("stmp_text").toString();
	}

	return Text;
}
//-----------------------------------------------------------------------------
