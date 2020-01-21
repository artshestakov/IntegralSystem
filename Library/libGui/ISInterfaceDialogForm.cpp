#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISInterfaceDialogForm::ISInterfaceDialogForm(QWidget *parent, Qt::WindowFlags Flags)
	: ISInterfaceForm(parent, Flags | ((Flags & Qt::WindowType_Mask) == 0 ? Qt::Dialog : Qt::WindowType(0)))
{
	Result = false;
}
//-----------------------------------------------------------------------------
ISInterfaceDialogForm::~ISInterfaceDialogForm()
{
	
}
//-----------------------------------------------------------------------------
bool ISInterfaceDialogForm::Exec()
{
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_ShowModal, true);

	if (!IsShowed())
	{
		show();
	}

	QEventLoop EventLoop;
	connect(this, &ISInterfaceDialogForm::Accept, &EventLoop, &QEventLoop::quit);
	EventLoop.exec(QEventLoop::DialogExec);

	setAttribute(Qt::WA_ShowModal, false);
	return Result;
}
//-----------------------------------------------------------------------------
bool ISInterfaceDialogForm::ExecAnimated()
{
	ShowAnimated();
	return Exec();
}
//-----------------------------------------------------------------------------
void ISInterfaceDialogForm::closeEvent(QCloseEvent *e)
{
	ISInterfaceForm::closeEvent(e);
	SetResult(Result);
	emit Accept();
}
//-----------------------------------------------------------------------------
void ISInterfaceDialogForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISInterfaceDialogForm::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();
	activateWindow();
	raise();
	ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
	setFocus(Qt::ActiveWindowFocusReason);
}
//-----------------------------------------------------------------------------
void ISInterfaceDialogForm::SetResult(bool result)
{
	Result = result;
}
//-----------------------------------------------------------------------------
