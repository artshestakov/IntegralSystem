#include "StdAfx.h"
#include "ISTaskDoneForm.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtonDialog.h"
#include "ISCore.h"
//-----------------------------------------------------------------------------
ISTaskDoneForm::ISTaskDoneForm(int task_id, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	TaskID = task_id;

	setWindowTitle(LOCALIZATION("Task.ResolutionTask"));
	setWindowIcon(BUFFER_ICONS("Task"));
	setFixedWidth(550);
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);
	ForbidResize();

	QVBoxLayout *LayoutGroupBox = new QVBoxLayout();

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LOCALIZATION("Task.SelectResolution"));
	GroupBox->setLayout(LayoutGroupBox);
	GetMainLayout()->addWidget(GroupBox);

	QRadioButton *RadioDone = new QRadioButton(GroupBox);
	RadioDone->setChecked(true);
	RadioDone->setText(LOCALIZATION("Task.SelectResolution.Done"));
	RadioDone->setCursor(CURSOR_POINTING_HAND);
	RadioDone->setObjectName(CONST_UID_TASK_STATUS_DONE);
	LayoutGroupBox->addWidget(RadioDone);

	QRadioButton *RadioNotDone = new QRadioButton(GroupBox);
	RadioNotDone->setText(LOCALIZATION("Task.SelectResolution.NotDone"));
	RadioNotDone->setCursor(CURSOR_POINTING_HAND);
	RadioNotDone->setObjectName(CONST_UID_TASK_STATUS_NOT_DONE);
	LayoutGroupBox->addWidget(RadioNotDone);

	QRadioButton *RadioCanceled = new QRadioButton(GroupBox);
	RadioCanceled->setText(LOCALIZATION("Task.SelectResolution.Canceled"));
	RadioCanceled->setCursor(CURSOR_POINTING_HAND);
	RadioCanceled->setObjectName(CONST_UID_TASK_STATUS_CANCELED);
	LayoutGroupBox->addWidget(RadioCanceled);

	ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(RadioDone);
	ButtonGroup->addButton(RadioNotDone);
	ButtonGroup->addButton(RadioCanceled);

	GetMainLayout()->addWidget(new QLabel(LOCALIZATION("Comment"), this));

	EditComment = new ISTextEdit(this);
	EditComment->setFixedHeight(180);
	EditComment->SetPlaceholderText(LOCALIZATION("Task.ResolutionComment"));
	GetMainLayout()->addWidget(EditComment);

	ISButtonDialog *ButtonDialog = new ISButtonDialog(this);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISTaskDoneForm::Apply);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISTaskDoneForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISTaskDoneForm::~ISTaskDoneForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskDoneForm::Apply()
{
	ISUuid SelectedStatusUID = ButtonGroup->checkedButton()->objectName();
	ISCore::TaskSetStatus(TaskID, SelectedStatusUID, EditComment->GetValue());
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
