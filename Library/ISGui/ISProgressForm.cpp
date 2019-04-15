#include "StdAfx.h"
#include "ISProgressForm.h"
#include "EXDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISControls.h"
#include "ISSystem.h"
#include "ISButtonClose.h"
//-----------------------------------------------------------------------------
ISProgressForm::ISProgressForm(int Minimum, int Maximum, QWidget *parent) : QProgressDialog(parent)
{
	Value = 0;
	setWindowTitle(LOCALIZATION("PleaseWait"));
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
	ISSystem::RepaintWidget(LabelStatus);
	qApp->processEvents();
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
	ISSystem::RepaintWidget(this);
	qApp->processEvents();
}
//-----------------------------------------------------------------------------
