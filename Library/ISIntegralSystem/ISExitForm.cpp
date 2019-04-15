#include "StdAfx.h"
#include "ISExitForm.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMemoryObjects.h"
//-----------------------------------------------------------------------------
ISExitForm::ISExitForm(QWidget *parent) : ISInterfaceDialogForm(parent)
{
	SelectedAction = ISNamespace::ExitFormAction::EFA_Unknown;

	setWindowTitle(LOCALIZATION("Exit"));
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);
	ForbidResize();

	QLabel *Label = new QLabel(this);
	Label->setText(LOCALIZATION("SelectAction") + ":");
	GetMainLayout()->addWidget(Label);

	QVBoxLayout *LayoutButtons = new QVBoxLayout();
	LayoutButtons->setContentsMargins(LAYOUT_MARGINS_NULL);
	LayoutButtons->setSpacing(0);
	GetMainLayout()->addLayout(LayoutButtons);

	QCommandLinkButton *ButtonLock = new QCommandLinkButton(this);
	ButtonLock->setText(LOCALIZATION("ExitForm.Lock"));
	ButtonLock->setToolTip(LOCALIZATION("ExitForm.Lock.ToolTip"));
	ButtonLock->setIcon(BUFFER_ICONS("Lock"));
	ButtonLock->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonLock);

	QCommandLinkButton *ButtonTray = new QCommandLinkButton(this);
	ButtonTray->setText(LOCALIZATION("ExitForm.Tray"));
	ButtonTray->setToolTip(LOCALIZATION("ExitForm.Tray.ToolTip"));
	ButtonTray->setIcon(BUFFER_ICONS("CurtailTray"));
	ButtonTray->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonTray);

	QCommandLinkButton *ButtonChangeUser = new QCommandLinkButton(this);
	ButtonChangeUser->setText(LOCALIZATION("ExitForm.ChangeUser"));
	ButtonChangeUser->setToolTip(LOCALIZATION("ExitForm.ChangeUser.ToolTip"));
	ButtonChangeUser->setIcon(BUFFER_ICONS("UserChange"));
	ButtonChangeUser->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonChangeUser);

	QCommandLinkButton *ButtonExit = new QCommandLinkButton(this);
	ButtonExit->setText(LOCALIZATION("ExitForm.Exit"));
	ButtonExit->setToolTip(LOCALIZATION("ExitForm.Exit.ToolTip"));
	ButtonExit->setIcon(BUFFER_ICONS("PowerOff"));
	ButtonExit->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonExit);

	QCommandLinkButton *ButtonClose = new QCommandLinkButton(this);
	ButtonClose->setText(LOCALIZATION("ExitForm.Close"));
	ButtonClose->setIcon(QIcon());
	ButtonClose->setCursor(CURSOR_POINTING_HAND);
	LayoutButtons->addWidget(ButtonClose);

	ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(ButtonLock, ISNamespace::EFA_Lock);
	ButtonGroup->addButton(ButtonTray, ISNamespace::EFA_Tray);
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
	QRect Rect = ISMemoryObjects::GetInstance().GetMainWindow()->property("geometry").toRect();
	
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
