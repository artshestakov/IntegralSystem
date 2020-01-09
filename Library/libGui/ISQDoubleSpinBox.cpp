#include "StdAfx.h"
#include "ISQDoubleSpinBox.h"
#include "ISDefines.h"
#include "ISContextMenuDouble.h"
#include "ISQLineEdit.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISQDoubleSpinBox::ISQDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    setMaximum(DOUBLE_MAXIMUM);

	ISQLineEdit *LineEdit = new ISQLineEdit(this);
	connect(LineEdit, &ISQLineEdit::ClearClicked, this, &ISQDoubleSpinBox::ClearClicked);
	setLineEdit(LineEdit);
}
//-----------------------------------------------------------------------------
ISQDoubleSpinBox::~ISQDoubleSpinBox()
{

}
//-----------------------------------------------------------------------------
void ISQDoubleSpinBox::SetVisibleClear(bool Visible)
{
	dynamic_cast<ISQLineEdit*>(lineEdit())->SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
QValidator::State ISQDoubleSpinBox::validate(QString &Input, int &Pos) const
{
	if (Input.contains("."))
	{
		Input.replace(".", SYMBOL_COMMA);
	}

	return QDoubleSpinBox::validate(Input, Pos);
}
//-----------------------------------------------------------------------------
void ISQDoubleSpinBox::contextMenuEvent(QContextMenuEvent *e)
{
	ISSystem::SetWaitGlobalCursor(true);
	ISContextMenuDouble ContextMenu(lineEdit(), lineEdit()->isReadOnly(), lineEdit()->isUndoAvailable(), lineEdit()->isRedoAvailable(), lineEdit()->hasSelectedText(), lineEdit()->echoMode(), lineEdit()->text().isEmpty(), value(), minimum(), maximum());
	connect(&ContextMenu, &ISContextMenuDouble::Delete, lineEdit(), &QLineEdit::del);
	connect(&ContextMenu, &ISContextMenuDouble::StepUp, this, &ISQDoubleSpinBox::stepUp);
	connect(&ContextMenu, &ISContextMenuDouble::StepDown, this, &ISQDoubleSpinBox::stepDown);
	ISSystem::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
