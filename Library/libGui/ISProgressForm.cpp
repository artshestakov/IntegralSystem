#include "ISProgressForm.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISGui.h"
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

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISProgressForm::CancelClicked);
	setCancelButton(ButtonClose);

	ISControls::SetBackgroundColorWidget(this, ISDefines::Gui::COLOR_WHITE);
}
//-----------------------------------------------------------------------------
ISProgressForm::~ISProgressForm()
{

}
//-----------------------------------------------------------------------------
void ISProgressForm::IncrementValue()
{
	setValue(++Value);
	ISGui::ProcessEvents();
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
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISProgressForm::CancelClicked()
{
	Canceled = true;
}
//-----------------------------------------------------------------------------
