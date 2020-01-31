#include "StdAfx.h"
#include "ISProcessForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISDefines.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISProcessForm::ISProcessForm(const QString &Text, QWidget *parent) : ISInterfaceForm(parent)
{
	setCursor(CURSOR_WAIT);
	setWindowTitle(LANG("PleaseWait") + "...");
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint);

	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(BUFFER_ICONS("Wait").pixmap(SIZE_32_32));
	Layout->addWidget(LabelImage);

	LabelText = new QLabel(this);
	Layout->addWidget(LabelText);

	Layout->addStretch();
}
//-----------------------------------------------------------------------------
ISProcessForm::~ISProcessForm()
{

}
//-----------------------------------------------------------------------------
void ISProcessForm::SetText(const QString &Text)
{
	LabelText->setText(SYMBOL_CIRCLE + SYMBOL_SPACE + Text + "...");
	ISGui::RepaintWidget(LabelText);
	adjustSize();
	ISGui::ProcessEvents();
}
//-----------------------------------------------------------------------------
void ISProcessForm::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
	ISInterfaceForm::show();
	update();
	ISGui::ProcessEvents();
	adjustSize();
	ISGui::RepaintWidget(this);
}
//-----------------------------------------------------------------------------
