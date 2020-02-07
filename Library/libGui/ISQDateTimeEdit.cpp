#include "ISQDateTimeEdit.h"
#include "ISStyleSheet.h"
#include "ISContextMenu.h"
#include "ISQLineEdit.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
ISQDateTimeEdit::ISQDateTimeEdit(QWidget *parent) : QDateTimeEdit(parent)
{
	setStyleSheet(STYLE_SHEET("ISDateTimeEdit"));
	setReadOnly(true);

	ISQLineEdit *LineEdit = new ISQLineEdit(this);
	LineEdit->SetVisibleClear(false);
	setLineEdit(LineEdit);
}
//-----------------------------------------------------------------------------
ISQDateTimeEdit::~ISQDateTimeEdit()
{

}
//-----------------------------------------------------------------------------
void ISQDateTimeEdit::contextMenuEvent(QContextMenuEvent *e)
{
	ISGui::SetWaitGlobalCursor(true);
	ISContextMenuDateTime ContextMenu(lineEdit(), lineEdit()->isReadOnly(), lineEdit()->isUndoAvailable(), lineEdit()->isRedoAvailable(), lineEdit()->hasSelectedText(), lineEdit()->echoMode(), lineEdit()->text().isEmpty());
	connect(&ContextMenu, &ISContextMenuDateTime::Delete, lineEdit(), &QLineEdit::del);
	connect(&ContextMenu, &ISContextMenuDateTime::StepUp, this, &ISQDateTimeEdit::stepUp);
	connect(&ContextMenu, &ISContextMenuDateTime::StepDown, this, &ISQDateTimeEdit::stepDown);
	ISGui::SetWaitGlobalCursor(false);
	ContextMenu.exec(e->globalPos());
}
//-----------------------------------------------------------------------------
