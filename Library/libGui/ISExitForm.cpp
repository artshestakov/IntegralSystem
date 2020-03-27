#include "ISExitForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISExitForm::ISExitForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	SelectedAction = ISNamespace::ExitFormAction::EFA_Unknown;

	setWindowTitle(LANG("Exit"));
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);
	ForbidResize();

	QLabel *Label = new QLabel(this);
	Label->setText(LANG("SelectAction") + ':');
	GetMainLayout()->addWidget(Label);

	QVBoxLayout *LayoutButtons = new QVBoxLayout();
	LayoutButtons->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutButtons->setSpacing(0);
	GetMainLayout()->addLayout(LayoutButtons);

	QCommandLinkButton *ButtonLock = new QCommandLinkButton(this);
	ButtonLock->setText(LANG("ExitForm.Lock"));
	ButtonLock->setToolTip(LANG("ExitForm.Lock.ToolTip"));
	ButtonLock->setIcon(BUFFER_ICONS("Lock"));
	ButtonLock->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonLock);

	QCommandLinkButton *ButtonChangeUser = new QCommandLinkButton(this);
	ButtonChangeUser->setText(LANG("ExitForm.ChangeUser"));
	ButtonChangeUser->setToolTip(LANG("ExitForm.ChangeUser.ToolTip"));
	ButtonChangeUser->setIcon(BUFFER_ICONS("UserChange"));
	ButtonChangeUser->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonChangeUser);

	QCommandLinkButton *ButtonExit = new QCommandLinkButton(this);
	ButtonExit->setText(LANG("ExitForm.Exit"));
	ButtonExit->setToolTip(LANG("ExitForm.Exit.ToolTip"));
	ButtonExit->setIcon(BUFFER_ICONS("PowerOff"));
	ButtonExit->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonExit);

	QCommandLinkButton *ButtonClose = new QCommandLinkButton(this);
	ButtonClose->setText(LANG("ExitForm.Close"));
	ButtonClose->setIcon(QIcon());
	ButtonClose->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonClose);

	ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(ButtonLock, ISNamespace::EFA_Lock);
	ButtonGroup->addButton(ButtonChangeUser, ISNamespace::EFA_ChangeUser);
	ButtonGroup->addButton(ButtonExit, ISNamespace::EFA_Exit);
	ButtonGroup->addButton(ButtonClose, ISNamespace::EFT_Close);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ISExitForm::ButtonClicked);
}
//-----------------------------------------------------------------------------
ISExitForm::~ISExitForm()
{

}
//-----------------------------------------------------------------------------
ISNamespace::ExitFormAction ISExitForm::GetSelectedAction()
{
	return SelectedAction;
}
//-----------------------------------------------------------------------------
void ISExitForm::showEvent(QShowEvent *e)
{
	ISInterfaceDialogForm::showEvent(e);
	QRect Rect;// = ISMemoryObjects::GetInstance().GetMainWindow()->property("geometry").toRect();
	
	int x = Rect.width() / 2;
	x = Rect.x() + x;
	x = x - (width() / 2);

	int y = Rect.height() / 2;
	y = Rect.y() + y;
	y = y - (height() / 2);

	move(x, y);
}
//-----------------------------------------------------------------------------
void ISExitForm::ButtonClicked(int ButtonID)
{
	SelectedAction = static_cast<ISNamespace::ExitFormAction>(ButtonID);

	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISExitForm::Close()
{
	SetResult(false);
	close();
}
//-----------------------------------------------------------------------------
