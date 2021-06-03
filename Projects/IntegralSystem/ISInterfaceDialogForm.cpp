#include "ISInterfaceDialogForm.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISInterfaceDialogForm::ISInterfaceDialogForm(bool HideCloseWindow, QWidget *parent)
    : ISInterfaceForm(parent, HideCloseWindow ? Qt::Dialog | Qt::WindowTitleHint : Qt::Dialog | Qt::WindowCloseButtonHint),
    Result(false)
{
    connect(this, &ISInterfaceDialogForm::Accept, &EventLoop, &QEventLoop::quit);
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
        ISGui::MoveWidgetToDesktop(this, ISNamespace::MoveWidgetDesktop::Center);
        show();
    }
    EventLoop.exec(QEventLoop::DialogExec);
    setAttribute(Qt::WA_ShowModal, false);
    return Result;
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
    ISGui::MoveWidgetToDesktop(this, ISNamespace::MoveWidgetDesktop::Center);
    setFocus(Qt::ActiveWindowFocusReason);
}
//-----------------------------------------------------------------------------
void ISInterfaceDialogForm::SetResult(bool result)
{
    Result = result;
}
//-----------------------------------------------------------------------------
