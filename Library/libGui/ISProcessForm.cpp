#include "ISProcessForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISProcessForm::ISProcessForm(const QString &Text, QWidget *parent) : ISInterfaceForm(parent)
{
	setCursor(CURSOR_WAIT);
	setWindowTitle(LANG("PleaseWait") + "...");
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint);

	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(BUFFER_ICONS("Wait").pixmap(ISDefines::Gui::SIZE_32_32));
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
	LabelText->setText(ISDefines::Core::SYMBOL_CIRCLE + SYMBOL_SPACE + Text + "...");
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
