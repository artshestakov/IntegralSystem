#include "ISPeriodForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
ISPeriodForm::ISPeriodForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("Period"));
	setWindowIcon(BUFFER_ICONS("Period"));
	GetMainLayout()->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);
	ForbidResize();

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QLabel *Label = new QLabel(this);
	Label->setText(LANG("SelectFrom") + ':');
	Label->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	Layout->addWidget(Label);

	RadioButtonCreate = new QRadioButton(this);
	RadioButtonCreate->setText(LANG("PeriodCreate"));
	connect(RadioButtonCreate, &QRadioButton::clicked, this, &ISPeriodForm::RadioChanged);
	Layout->addWidget(RadioButtonCreate);

	RadioButtonEdit = new QRadioButton(this);
	RadioButtonEdit->setText(LANG("PeriodUpdate"));
	connect(RadioButtonEdit, &QRadioButton::clicked, this, &ISPeriodForm::RadioChanged);
	Layout->addWidget(RadioButtonEdit);

	Layout->addStretch();

	RangeDateEdit = new ISRangeDateEdit(this);
	RangeDateEdit->setEnabled(false);
	GetMainLayout()->addWidget(RangeDateEdit);

	ButtonDialog = new ISButtonDialog(this);
	ButtonDialog->SetApplyEnabled(false);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISPeriodForm::PeriodInstall);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISPeriodForm::close);
	GetMainLayout()->addWidget(ButtonDialog);

	ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(RadioButtonCreate, ISNamespace::PT_CreationDate);
	ButtonGroup->addButton(RadioButtonEdit, ISNamespace::PT_UpdationDate);
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
ISRangeStruct ISPeriodForm::GetRange() const
{
	return RangeDateEdit->GetValue();
}
//-----------------------------------------------------------------------------
void ISPeriodForm::SetRange(ISNamespace::PeriodType PeriodType, const ISRangeStruct &Range)
{
	if (PeriodType != ISNamespace::PT_UnknownDate && Range.IsValid())
	{
		ButtonGroup->button(PeriodType)->setChecked(true);
		RangeDateEdit->setEnabled(true);
		RangeDateEdit->SetValue(Range);
		ButtonDialog->SetApplyEnabled(true);
	}
}
//-----------------------------------------------------------------------------
void ISPeriodForm::RadioChanged()
{
	RangeDateEdit->setEnabled(true);
	ButtonDialog->SetApplyEnabled(true);
}
//-----------------------------------------------------------------------------
void ISPeriodForm::PeriodInstall()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
