#include "ISInterfaceDialogForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISInterfaceDialogForm::ISInterfaceDialogForm()
	: ISInterfaceForm(nullptr, Qt::Dialog | Qt::WindowCloseButtonHint),
	Result(false)
{
	
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
		adjustSize();
		ISGui::MoveWidgetToDesktop(this, ISNamespace::MWD_Center);
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
void ISInterfaceDialogForm::closeEvent(QCloseEvent *CloseEvent)
{
	ISInterfaceForm::closeEvent(CloseEvent);
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
