#include "ISContextMenuDouble.h"
#include "ISControls.h"
//-----------------------------------------------------------------------------
ISContextMenuDouble::ISContextMenuDouble(QWidget *ParentEdit, bool ReadOnly, bool UndoAvailable, bool RedoAvailable, bool HasSelectedText, QLineEdit::EchoMode EchoMode, bool Empty, double Value, double Minimum, double Maximum)
	: ISContextMenuBase(ParentEdit, ReadOnly, UndoAvailable, RedoAvailable, HasSelectedText, EchoMode, Empty)
{
	QAction *ActionStepUp = ISControls::GetActionContextStepUp(this);
	ActionStepUp->setEnabled(Value != Maximum);
	connect(ActionStepUp, &QAction::triggered, this, &ISContextMenuDouble::StepUp);
	addAction(ActionStepUp);

	QAction *ActionStepDown = ISControls::GetActionContextStepDown(this);
	ActionStepDown->setEnabled(Value != Minimum);
	connect(ActionStepDown, &QAction::triggered, this, &ISContextMenuDouble::StepDown);
	addAction(ActionStepDown);
}
//-----------------------------------------------------------------------------
ISContextMenuDouble::~ISContextMenuDouble()
{

}
//-----------------------------------------------------------------------------
