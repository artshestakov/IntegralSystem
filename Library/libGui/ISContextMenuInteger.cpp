#include "ISContextMenuInteger.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISContextMenuInteger::ISContextMenuInteger(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, int Value, int Minimum, int Maximum)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionStepUp = ISControls::GetActionContextStepUp(this);
	ActionStepUp->setEnabled(Value != Maximum);
	connect(ActionStepUp, &QAction::triggered, this, &ISContextMenuInteger::StepUp);
	addAction(ActionStepUp);

	QAction *ActionStepDown = ISControls::GetActionContextStepDown(this);
	ActionStepDown->setEnabled(Value != Minimum);
	connect(ActionStepDown, &QAction::triggered, this, &ISContextMenuInteger::StepDown);
	addAction(ActionStepDown);
}
//-----------------------------------------------------------------------------
ISContextMenuInteger::~ISContextMenuInteger()
{

}
//-----------------------------------------------------------------------------
