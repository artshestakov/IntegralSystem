#include "ISProcessForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISProcessForm::ISProcessForm(const QString &LabelText, QWidget *parent)
	: ISInterfaceForm(parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
{
	setCursor(CURSOR_WAIT);
	setWindowTitle(LANG("PleaseWait"));
	setSizePolicy(QSizePolicy::Maximum, sizePolicy().verticalPolicy());
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	QLabel *LabelImage = new QLabel(this);
	LabelImage->setPixmap(BUFFER_ICONS("Wait").pixmap(ISDefines::Gui::SIZE_32_32));
	Layout->addWidget(LabelImage);

	Label = new QLabel(this);
	Label->setText(LabelText);
	Label->setSizePolicy(QSizePolicy::Maximum, Label->sizePolicy().verticalPolicy());
	Layout->addWidget(Label);

	Layout->addStretch();
}
//-----------------------------------------------------------------------------
ISProcessForm::~ISProcessForm()
{

}
//-----------------------------------------------------------------------------
void ISProcessForm::SetText(const QString &LabelText)
{
	//ѕоследовательность вызова функций ниже ни в коем случае не мен€ть, иначе
	//возникнут проблемы с изменением размера формы после изменени€ текста в QLabel
	Label->setText(LabelText);
	Label->adjustSize();
	ISGui::RepaintWidget(Label);
	adjustSize();
	ISGui::RepaintWidget(this);
	ISSystem::ProcessEvents();
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
}
//-----------------------------------------------------------------------------
void ISProcessForm::showEvent(QShowEvent *ShowEvent)
{
	adjustSize();
	ISGui::RepaintWidget(this);
	ISSystem::ProcessEvents();
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
	ISInterfaceForm::showEvent(ShowEvent);
}
//-----------------------------------------------------------------------------
