#include "ISProgressForm.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISButtonClose.h"
//-----------------------------------------------------------------------------
ISProgressForm::ISProgressForm(int Minimum, int Maximum, QWidget *parent) : QProgressDialog(parent)
{
	Value = 0;
	setWindowTitle(LANG("PleaseWait"));
	setWindowModality(Qt::WindowModal);
	setFixedSize(SIZE_PROGRESS_FORM);
	setMinimum(Minimum);
	setMaximum(Maximum);

	ISControls::SetBackgroundColorWidget(this, COLOR_WHITE);

	LabelStatus = new QLabel(this);
	LabelStatus->setAlignment(Qt::AlignLeft);
	setLabel(LabelStatus);

	QMargins Margins = LabelStatus->contentsMargins();
	Margins.setTop(10);
	LabelStatus->setContentsMargins(Margins);
	
	setCancelButton(dynamic_cast<QPushButton*>(new ISButtonClose(this)));
}
//-----------------------------------------------------------------------------
ISProgressForm::~ISProgressForm()
{

}
//-----------------------------------------------------------------------------
void ISProgressForm::SetText(const QString &Text)
{
	setLabelText(Text);
	ISGui::RepaintWidget(LabelStatus);
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISProgressForm::AddOneValue()
{
	Value++;
	setValue(Value);
}
//-----------------------------------------------------------------------------
void ISProgressForm::showEvent(QShowEvent *e)
{
	QProgressDialog::showEvent(e);
	ISGui::RepaintWidget(this);
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
