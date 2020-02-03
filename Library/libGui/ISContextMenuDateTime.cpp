#include "ISContextMenuDateTime.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISContextMenuDateTime::ISContextMenuDateTime(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionStepUp = ISControls::GetActionContextStepUp(this);
	connect(ActionStepUp, &QAction::triggered, this, &ISContextMenuDateTime::StepUp);
	addAction(ActionStepUp);

	QAction *ActionStepDown = ISControls::GetActionContextStepDown(this);
	connect(ActionStepDown, &QAction::triggered, this, &ISContextMenuDateTime::StepDown);
	addAction(ActionStepDown);
}
//-----------------------------------------------------------------------------
ISContextMenuDateTime::~ISContextMenuDateTime()
{

}
//-----------------------------------------------------------------------------
