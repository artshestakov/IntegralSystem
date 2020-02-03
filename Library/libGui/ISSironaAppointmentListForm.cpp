#include "ISSironaAppointmentListForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMessageBox.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QU_ADMISSION_FINISHED = PREPARE_QUERY("UPDATE appointment SET "
													 "apnm_finished = true "
													 "WHERE apnm_id = :ObjectID");
//-----------------------------------------------------------------------------
ISSironaAppointmentListForm::ISSironaAppointmentListForm(QWidget *parent) : ISListBaseForm("Appointment", parent)
{
	QAction *ActionFinished = new QAction(this);
	ActionFinished->setText(LANG("Sirona.AdmissionFinished"));
	ActionFinished->setToolTip(LANG("Sirona.AdmissionFinished"));
	ActionFinished->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ActionFinished, &QAction::triggered, this, &ISSironaAppointmentListForm::AdmissionFinished);
	AddAction(ActionFinished, true, true);

	RadioAll = new QRadioButton(this);
	RadioAll->setText(LANG("Sirona.Admission.Filter.All"));
	GetToolBar()->addWidget(RadioAll);

	RadioActual = new QRadioButton(this);
	RadioActual->setChecked(true);
	RadioActual->setText(LANG("Sirona.Admission.Filter.Actial"));
	GetToolBar()->addWidget(RadioActual);

	RadioFinished = new QRadioButton(this);
	RadioFinished->setText(LANG("Sirona.Admission.Filter.Finished"));
	GetToolBar()->addWidget(RadioFinished);

	QButtonGroup *ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(RadioAll);
	ButtonGroup->addButton(RadioActual);
	ButtonGroup->addButton(RadioFinished);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &ISSironaAppointmentListForm::FilterChange);
}
//-----------------------------------------------------------------------------
ISSironaAppointmentListForm::~ISSironaAppointmentListForm()
{

}
//-----------------------------------------------------------------------------
void ISSironaAppointmentListForm::AdmissionFinished()
{
	QString PatientName = GetCurrentRecordValue("Patient").toString();
	bool Finished = GetCurrentRecordValue("Finished").toBool();
	if (Finished) //Если приём уже окончен
	{
		ISMessageBox::ShowInformation(this, LANG("Sirona.Message.Information.AdmissionAlreadyFinished").arg(PatientName));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Sirona.Message.Question.AdmissionFinished").arg(PatientName).arg(GetCurrentRecordValue("DateTime").toDate().toString(DATE_FORMAT_V2))))
	{
		ISQuery qUpdate(QU_ADMISSION_FINISHED);
		qUpdate.BindValue(":ObjectID", GetObjectID());
		if (qUpdate.Execute())
		{
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ISSironaAppointmentListForm::FilterChange(QAbstractButton *AbstractButton)
{
	if (AbstractButton == RadioAll)
	{
		GetQueryModel()->ClearClassFilter();
	}
	else if (AbstractButton == RadioActual)
	{
		GetQueryModel()->SetClassFilter("NOT apnm_finished");
	}
	else if (AbstractButton == RadioFinished)
	{
		GetQueryModel()->SetClassFilter("apnm_finished");
	}

	Update();
}
//-----------------------------------------------------------------------------
