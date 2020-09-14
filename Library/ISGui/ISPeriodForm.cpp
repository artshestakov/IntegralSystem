#include "ISPeriodForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISPeriodForm::ISPeriodForm()
	: ISInterfaceDialogForm(),
	ButtonGroup(new QButtonGroup(this))
{
	setWindowTitle(LANG("Period"));
	setWindowIcon(BUFFER_ICONS("Period"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	resize(400, 150);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QLabel *Label = new QLabel(this);
	Label->setText(LANG("SelectFrom") + ':');
	Label->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	Layout->addWidget(Label);

	RadioButtonCreate = new QRadioButton(this);
	RadioButtonCreate->setText(LANG("PeriodCreate"));
	connect(RadioButtonCreate, &QRadioButton::clicked, this, &ISPeriodForm::RadioChanged);
	Layout->addWidget(RadioButtonCreate);
	ButtonGroup->addButton(RadioButtonCreate, ISNamespace::PT_CreationDate);

	RadioButtonEdit = new QRadioButton(this);
	RadioButtonEdit->setText(LANG("PeriodUpdate"));
	connect(RadioButtonEdit, &QRadioButton::clicked, this, &ISPeriodForm::RadioChanged);
	Layout->addWidget(RadioButtonEdit);
	ButtonGroup->addButton(RadioButtonEdit, ISNamespace::PT_UpdationDate);

	Layout->addStretch();

	QHBoxLayout *LayoutEdits = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutEdits);

	LayoutEdits->addWidget(new QLabel(LANG("FieldRange.DateTime.Begin"), this));

	EditBegin = new ISDateEdit(this);
	LayoutEdits->addWidget(EditBegin);
	
	LayoutEdits->addWidget(new QLabel(LANG("FieldRange.DateTime.End"), this));

	EditEnd = new ISDateEdit(this);
	LayoutEdits->addWidget(EditEnd);

	LayoutEdits->addStretch();

	GetMainLayout()->addStretch();

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutBottom);

	ISPushButton *ButtonPeriod = new ISPushButton(LANG("Range.Date.Select"), this);
	ButtonPeriod->setMenu(new QMenu(ButtonPeriod));
	QAction *ActionToday = ButtonPeriod->menu()->addAction(LANG("Range.Date.Today"), this, &ISPeriodForm::PeriodSelected);
	QAction *ActionYesterday = ButtonPeriod->menu()->addAction(LANG("Range.Date.Yesterday"), this, &ISPeriodForm::PeriodSelected);
	QAction *ActionCurrentMonth = ButtonPeriod->menu()->addAction(LANG("Range.Date.CurrentMonth"), this, &ISPeriodForm::PeriodSelected);
	QAction *ActionPreviousMonth = ButtonPeriod->menu()->addAction(LANG("Range.Date.PreviousMonth"), this, &ISPeriodForm::PeriodSelected);
	QAction *ActionCurrentYear = ButtonPeriod->menu()->addAction(LANG("Range.Date.CurrentYear"), this, &ISPeriodForm::PeriodSelected);
	QAction *ActionPreviousYear = ButtonPeriod->menu()->addAction(LANG("Range.Date.PreviousYear"), this, &ISPeriodForm::PeriodSelected);
	LayoutBottom->addWidget(ButtonPeriod);

	ActionToday->setData(ISNamespace::RDP_Today);
	ActionYesterday->setData(ISNamespace::RDP_Yesterday);
	ActionCurrentMonth->setData(ISNamespace::RDP_CurrentMonth);
	ActionPreviousMonth->setData(ISNamespace::RDP_PreviousMonth);
	ActionCurrentYear->setData(ISNamespace::RDP_CurrentYear);
	ActionPreviousYear->setData(ISNamespace::RDP_PreviousYear);

	LayoutBottom->addStretch();

	ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISPeriodForm::PeriodInstall);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISPeriodForm::close);
	LayoutBottom->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISPeriodForm::~ISPeriodForm()
{

}
//-----------------------------------------------------------------------------
ISNamespace::PeriodType ISPeriodForm::GetPeriodType() const
{
	return qvariant_cast<ISNamespace::PeriodType>(ButtonGroup->checkedId());
}
//-----------------------------------------------------------------------------
QDate ISPeriodForm::GetBegin() const
{
	return EditBegin->GetValue().toDate();
}
//-----------------------------------------------------------------------------
QDate ISPeriodForm::GetEnd() const
{
	return EditEnd->GetValue().toDate();
}
//-----------------------------------------------------------------------------
void ISPeriodForm::SetRange(ISNamespace::PeriodType PeriodType, const QDate &DateBegin, const QDate &DateEnd)
{
	if (PeriodType != ISNamespace::PT_UnknownDate && DateBegin.isValid() && DateEnd.isValid())
	{
		ButtonGroup->button(PeriodType)->setChecked(true);
		EditBegin->setEnabled(true);
		EditEnd->setEnabled(true);
		EditBegin->SetValue(DateBegin);
		EditEnd->SetValue(DateEnd);
		ButtonDialog->SetApplyEnabled(true);
	}
}
//-----------------------------------------------------------------------------
void ISPeriodForm::RadioChanged()
{
	EditBegin->setEnabled(true);
	EditEnd->setEnabled(true);
	ButtonDialog->SetApplyEnabled(true);
}
//-----------------------------------------------------------------------------
void ISPeriodForm::PeriodSelected()
{
	ISNamespace::RangeDatePeriod SelectedPeriod = qvariant_cast<ISNamespace::RangeDatePeriod>(dynamic_cast<QAction*>(sender())->data());
	if (SelectedPeriod == ISNamespace::RDP_Today)
	{
		EditBegin->SetValue(QDate::currentDate());
		EditEnd->SetValue(QDate::currentDate());
	}
	else if (SelectedPeriod == ISNamespace::RDP_Yesterday)
	{
		EditBegin->SetValue(QDate::currentDate().addDays(-1));
		EditEnd->SetValue(QDate::currentDate().addDays(-1));
	}
	else if (SelectedPeriod == ISNamespace::RDP_CurrentMonth)
	{
		EditBegin->SetValue(QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1));
		EditEnd->SetValue(QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().daysInMonth()));
	}
	else if (SelectedPeriod == ISNamespace::RDP_PreviousMonth)
	{
		QDate Date = QDate::currentDate().addMonths(-1);
		EditBegin->SetValue(QDate(Date.year(), Date.month(), 1));
		EditEnd->SetValue(QDate(Date.year(), Date.month(), Date.daysInMonth()));
	}
	else if (SelectedPeriod == ISNamespace::RDP_CurrentYear)
	{
		EditBegin->SetValue(QDate(QDate::currentDate().year(), 1, 1));
		EditEnd->SetValue(QDate(QDate::currentDate().year(), 12, 31));
	}
	else if (SelectedPeriod == ISNamespace::RDP_PreviousYear)
	{
		QDate Date = QDate::currentDate().addYears(-1);
		EditBegin->SetValue(QDate(Date.year(), 1, 1));
		EditEnd->SetValue(QDate(Date.year(), 12, 31));
	}
	EditBegin->SetFocus();
}
//-----------------------------------------------------------------------------
void ISPeriodForm::PeriodInstall()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
