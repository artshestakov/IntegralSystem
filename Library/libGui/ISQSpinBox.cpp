#include "ISQSpinBox.h"
#include "ISContextMenu.h"
#include "ISQLineEdit.h"
#include "ISGui.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISQSpinBox::ISQSpinBox(QWidget *parent) : QSpinBox(parent)
{
	setMaximum(MAXIMUM_INTEGER);

	ISQLineEdit *LineEdit = new ISQLineEdit(this);
	connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISQSpinBox::ClearClicked);
	setLineEdit(LineEdit);
}
//-----------------------------------------------------------------------------
ISQSpinBox::~ISQSpinBox()
{

}
//-----------------------------------------------------------------------------
void ISQSpinBox::SetCursorPosition(int Position)
{
	lineEdit()->setCursorPosition(Position);
}
//-----------------------------------------------------------------------------
void ISQSpinBox::SetVisibleClear(bool Visible)
{
	dynamic_cast<ISQLineEdit*>(lineEdit())->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISQSpinBox::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuInteger ContextMenu(lineEdit(), lineEdit()->isReadOnly(), lineEdit()->isUndoAvailable(), lineEdit()->isRedoAvailable(), lineEdit()->hasSelectedText(), lineEdit()->echoMode(), lineEdit()->text().isEmpty(), value(), minimum(), maximum());
	connect(&ContextMenu, &ISContextMenuInteger::Delete, lineEdit(), &QLineEdit::del);
	connect(&ContextMenu, &ISContextMenuInteger::StepUp, this, &ISQSpinBox::stepUp);
	connect(&ContextMenu, &ISContextMenuInteger::StepDown, this, &ISQSpinBox::stepDown);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
