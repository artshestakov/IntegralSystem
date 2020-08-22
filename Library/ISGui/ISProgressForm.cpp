#include "ISProgressForm.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISSystem.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
ISProgressForm::ISProgressForm(int Maximum, const QString &LabelText, QWidget *parent, const QString &TitleText)
	: QProgressDialog(LabelText, QString(), 0, Maximum, parent),
	Value(0),
	Canceled(false)
{
	setWindowTitle(TitleText.isEmpty() ? LANG("PleaseWait") : TitleText);
	setWindowModality(Qt::WindowModal);
	setFixedSize(ISDefines::Gui::SIZE_PROGRESS_FORM);
	setValue(Value);
	setMinimumDuration(0);
	setAutoClose(true);

	ISPushButton *ButtonClose = new ISPushButton(BUFFER_ICONS("Close"), LANG("Close"), this);
	connect(ButtonClose, &ISPushButton::clicked, this, &ISProgressForm::CancelClicked);
	setCancelButton(ButtonClose);

	ISControls::SetBackgroundColorWidget(this, ISDefines::Gui::COLOR_WHITE);
}
//-----------------------------------------------------------------------------
ISProgressForm::~ISProgressForm()
{

}
//-----------------------------------------------------------------------------
void ISProgressForm::IncrementValue(const QString &LabelText)
{
	if (!LabelText.isEmpty())
	{
		setLabelText(LabelText);
	}
	IncrementValue();
}
//-----------------------------------------------------------------------------
void ISProgressForm::IncrementValue()
{
	setValue(++Value);
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
bool ISProgressForm::WasCanceled() const
{
	return Canceled;
}
//-----------------------------------------------------------------------------
void ISProgressForm::SetCanceled(bool canceled)
{
	Canceled = canceled;
}
//-----------------------------------------------------------------------------
void ISProgressForm::showEvent(QShowEvent *e)
{
	QProgressDialog::showEvent(e);
	ISSystem::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISProgressForm::CancelClicked()
{
	Canceled = true;
}
//-----------------------------------------------------------------------------
