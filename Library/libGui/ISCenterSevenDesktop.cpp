#include "ISCenterSevenDesktop.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISButtons.h"
#include "ISSystem.h"
#include "ISCenterSevenAdmissionObjectForm.h"
#include "ISProperty.h"
#include "ISControls.h"
#include "ISObjects.h"
#include "ISGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_BRANCHES = PREPARE_QUERY2("SELECT brch_id, brch_name "
										   "FROM branches "
										   "WHERE NOT brch_isdeleted "
										   "ORDER BY brch_name");
//-----------------------------------------------------------------------------
static QString QS_DOCTORS = PREPARE_QUERY2("SELECT dctr_id, dctr_fio "
										  "FROM doctor "
										  "WHERE NOT dctr_isdeleted "
										  "ORDER BY dctr_fio");
//-----------------------------------------------------------------------------
ISCenterSevenDesktop::ISCenterSevenDesktop(QWidget *parent) : QWidget(parent)
{
	AdmissionListForm = nullptr;
	CurrentDoctorID = 0;

	MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	setLayout(MainLayout);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	LayoutTitle->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetTitle = new QWidget(this);
	WidgetTitle->setLayout(LayoutTitle);
	WidgetTitle->setSizePolicy(WidgetTitle->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	MainLayout->addWidget(WidgetTitle);

	LabelDayWeek = new QLabel(WidgetTitle);
	LabelDayWeek->setText(LANG("Today") + ": " + ISSystem::GetCurrentDayOfWeekName());
	LabelDayWeek->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	ISGui::SetFontWidgetUnderline(LabelDayWeek, true);
	LayoutTitle->addWidget(LabelDayWeek);

	ISPushButton *ButtonPreviousDay = new ISPushButton(WidgetTitle);
	ButtonPreviousDay->setText(LANG("PreviousDay"));
	ButtonPreviousDay->setIcon(BUFFER_ICONS("Arrow.Left"));
	connect(ButtonPreviousDay, &ISPushButton::clicked, this, &ISCenterSevenDesktop::PreviousDay);
	LayoutTitle->addWidget(ButtonPreviousDay);

	DateEdit = new ISDateEdit(WidgetTitle);
	DateEdit->SetChecked(true);
	DateEdit->SetVisibleCheck(false);
	connect(DateEdit, &ISDateEdit::ValueChange, this, &ISCenterSevenDesktop::DateChanged);
	LayoutTitle->addWidget(DateEdit);

	ISPushButton *ButtonNextDay = new ISPushButton(WidgetTitle);
	ButtonNextDay->setText(LANG("NextDay"));
	ButtonNextDay->setIcon(BUFFER_ICONS("Arrow.Right"));
	connect(ButtonNextDay, &ISPushButton::clicked, this, &ISCenterSevenDesktop::NextDay);
	LayoutTitle->addWidget(ButtonNextDay);

	LayoutTitle->addStretch();

	ISPushButton *ButtonWaitAdmission = new ISPushButton(this);
	ButtonWaitAdmission->setText(LANG("CenterSeven.AddWaitAdmission"));
	ButtonWaitAdmission->setIcon(BUFFER_ICONS("Wait"));
	connect(ButtonWaitAdmission, &ISPushButton::clicked, this, &ISCenterSevenDesktop::AddWaitAdmission);
	LayoutTitle->addWidget(ButtonWaitAdmission);

	QLabel *LabelBranch = new QLabel(this);
	LabelBranch->setText(LANG("CenterSeven.Branch") + ':');
	LayoutTitle->addWidget(LabelBranch);

	ComboBranch = new ISComboEdit(this);
	ComboBranch->SetEditable(false);
	ComboBranch->setCursor(CURSOR_POINTING_HAND);
	connect(ComboBranch, &ISComboEdit::DataChanged, this, &ISCenterSevenDesktop::BranchChanged);
	LayoutTitle->addWidget(ComboBranch);
	LoadBranches();

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	MainLayout->addWidget(ToolBar, 0, Qt::AlignTop);

	ActionDoctors = new QAction(ToolBar);
	ActionDoctors->setText(LANG("CenterSeven.AllDoctors"));
	ActionDoctors->setToolTip(LANG("CenterSeven.AllDoctors"));
	ActionDoctors->setCheckable(true);
	ActionDoctors->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("AllDoctors"));
	ActionDoctors->setMenu(new QMenu(ToolBar));
	connect(ActionDoctors, &QAction::triggered, this, &ISCenterSevenDesktop::AllDoctorClicked);
	ToolBar->addAction(ActionDoctors);

	ToolBar->addSeparator();

	ActionGroup = new QActionGroup(this);

	ISQuery qSelectDoctors(QS_DOCTORS);
	if (qSelectDoctors.Execute())
	{
		while (qSelectDoctors.Next())
		{
			int DoctorID = qSelectDoctors.ReadColumn("dctr_id").toInt();
			QString DoctorFIO = qSelectDoctors.ReadColumn("dctr_fio").toString();

			QAction *ActionDoctor = new QAction(ToolBar);
			ActionDoctor->setText(DoctorFIO);
			ActionDoctor->setIcon(BUFFER_ICONS("User"));
			ActionDoctor->setCheckable(true);
			ActionDoctor->setData(DoctorID);
			connect(ActionDoctor, &QAction::triggered, this, &ISCenterSevenDesktop::DoctorClicked);
			ToolBar->addAction(ActionDoctor);
			ActionDoctors->menu()->addAction(ActionDoctor);
			
			ActionGroup->addAction(ActionDoctor);
			dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionDoctor))->setCursor(CURSOR_POINTING_HAND);
		}
	}

	MainLayout->addWidget(ISControls::CreateHorizontalLine(this));

	AdmissionListForm = new ISCenterSevenAdmissionListForm(this);
	AdmissionListForm->SetBranch(ComboBranch->GetValue().toInt());
	AdmissionListForm->SetDate(DateEdit->GetValue().toDate());
	AdmissionListForm->SetDoctor(0);
	connect(AdmissionListForm, &ISCenterSevenAdmissionListForm::AddFormFromTab, this, &ISCenterSevenDesktop::CreateAdmissionObjectForm);
	MainLayout->addWidget(AdmissionListForm);
}
//-----------------------------------------------------------------------------
ISCenterSevenDesktop::~ISCenterSevenDesktop()
{

}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::PreviousDay()
{
	DateEdit->SetValue(DateEdit->GetValue().toDate().addDays(-1));
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::NextDay()
{
	DateEdit->SetValue(DateEdit->GetValue().toDate().addDays(+1));
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::AddWaitAdmission()
{
	ISGui::CreateObjectForm(ISNamespace::OFT_New, "WaitAdmission")->show();
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::LoadBranches()
{
	int CurrentBranchID = ISProperty::GetInstance().GetValue("BranchID").toInt();
	ISQuery qSelect(QS_BRANCHES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int BranchID = qSelect.ReadColumn("brch_id").toInt();
			QString BranchName = qSelect.ReadColumn("brch_name").toString();

			if (BranchID == CurrentBranchID)
			{
				BranchName += " (" + LANG("CenterSeven.CurrentBranch") + ')';
			}

			ComboBranch->AddItem(BranchName, BranchID);
		}
	}

	ComboBranch->SetValue(CurrentBranchID);
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::DateChanged(const QVariant &value)
{
	if (AdmissionListForm)
	{
		AdmissionListForm->SetDate(value.toDate());
		AdmissionListForm->Update();
	}
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::BranchChanged()
{
	if (AdmissionListForm)
	{
		int SelectedBranchID = ComboBranch->GetValue().toInt();
		if (SelectedBranchID == ISProperty::GetInstance().GetValue("BranchID").toInt()) //Если выбран текущий филиал
		{
			AdmissionListForm->SetEnabledActions(true);
		}
		else
		{
			AdmissionListForm->SetEnabledActions(false);
		}

		AdmissionListForm->SetBranch(SelectedBranchID);
		AdmissionListForm->Update();
	}
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::AllDoctorClicked()
{
	ActionDoctors->setChecked(true);
	ActionDoctors->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	for (QAction *ActionDoctor : ActionGroup->actions())
	{
		ActionDoctor->setChecked(false);
		ActionDoctor->setFont(ISDefines::Gui::FONT_APPLICATION);
	}

	AdmissionListForm->SetDoctor(0);
	AdmissionListForm->Update();
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::DoctorClicked()
{
	ActionDoctors->setChecked(false);
	ActionDoctors->setFont(ISDefines::Gui::FONT_APPLICATION);

	for (QAction *ActionDoctor : ActionGroup->actions())
	{
		ActionDoctor->setFont(ISDefines::Gui::FONT_APPLICATION);
	}

	QAction *ActionSender = dynamic_cast<QAction*>(sender());
	ActionSender->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	CurrentDoctorID = ActionSender->data().toInt();
	AdmissionListForm->SetDoctor(CurrentDoctorID);
	AdmissionListForm->Update();
}
//-----------------------------------------------------------------------------
void ISCenterSevenDesktop::CreateAdmissionObjectForm(QWidget *ObjectFormBase)
{
	ISCenterSevenAdmissionObjectForm *AdmissionObjectForm = dynamic_cast<ISCenterSevenAdmissionObjectForm*>(ObjectFormBase);
	AdmissionObjectForm->SetDate(DateEdit->GetValue().toDate());
	AdmissionObjectForm->SetDoctor(CurrentDoctorID);
	AdmissionObjectForm->setParent(nullptr);
	AdmissionObjectForm->resize(ISDefines::Gui::SIZE_TASK_OBJECT_FORM);
	AdmissionObjectForm->show();
}
//-----------------------------------------------------------------------------
