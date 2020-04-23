#include "ISCenterSevenStatusAdmissionForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISControls.h"
#include "ISButtonDialog.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_ADMISSION_STATUS = PREPARE_QUERY2("SELECT adst_uid, adst_name "
												   "FROM admissionstatus "
												   "WHERE NOT adst_isdeleted "
												   "ORDER BY adst_order");
//-----------------------------------------------------------------------------
ISCenterSevenStatusAdmissionForm::ISCenterSevenStatusAdmissionForm(const ISUuid &CurrentStatusUID, const QString &Comment) : ISInterfaceDialogForm()
{
	setWindowTitle(LANG("CenterSeven.ChangeStatusAdmission"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	ButtonGroup = new QButtonGroup(this);

	ISQuery qSelect(QS_ADMISSION_STATUS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid StatusUID = qSelect.ReadColumn("adst_uid");
			QString StatusName = qSelect.ReadColumn("adst_name").toString();

			QRadioButton *RadioButton = new QRadioButton(this);
			RadioButton->setText(StatusName);
			RadioButton->setToolTip(StatusName);
			RadioButton->setCursor(CURSOR_POINTING_HAND);
			RadioButton->setObjectName(StatusUID);
			GetMainLayout()->addWidget(RadioButton);

			ButtonGroup->addButton(RadioButton);

			if (CurrentStatusUID == StatusUID)
			{
				RadioButton->setChecked(true);
			}
		}
	}

	EditComment = new ISLineEdit(this);
	EditComment->SetValue(Comment);
	EditComment->SetPlaceholderText(LANG("CenterSeven.ChangeStatusComment"));
	GetMainLayout()->addWidget(EditComment);

	GetMainLayout()->addStretch();

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	ISButtonDialog *ButtonDialog = new ISButtonDialog(this);
	connect(ButtonDialog, &ISButtonDialog::Apply, this, &ISCenterSevenStatusAdmissionForm::StatusChange);
	connect(ButtonDialog, &ISButtonDialog::Close, this, &ISCenterSevenStatusAdmissionForm::close);
	GetMainLayout()->addWidget(ButtonDialog);
}
//-----------------------------------------------------------------------------
ISCenterSevenStatusAdmissionForm::~ISCenterSevenStatusAdmissionForm()
{

}
//-----------------------------------------------------------------------------
ISUuid ISCenterSevenStatusAdmissionForm::GetStatusUID() const
{
	return ButtonGroup->checkedButton()->objectName();
}
//-----------------------------------------------------------------------------
QString ISCenterSevenStatusAdmissionForm::GetComment() const
{
	return EditComment->GetValue().toString();
}
//-----------------------------------------------------------------------------
void ISCenterSevenStatusAdmissionForm::EnterClicked()
{
	StatusChange();
}
//-----------------------------------------------------------------------------
void ISCenterSevenStatusAdmissionForm::StatusChange()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
