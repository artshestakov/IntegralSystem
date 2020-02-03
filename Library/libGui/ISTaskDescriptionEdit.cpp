#include "ISTaskDescriptionEdit.h"
//-----------------------------------------------------------------------------
ISTaskDescriptionEdit::ISTaskDescriptionEdit(QWidget *parent) : ISTextEdit(parent)
{
	GetTextEdit()->setMaximumHeight(75);
}
//-----------------------------------------------------------------------------
ISTaskDescriptionEdit::~ISTaskDescriptionEdit()
{

}
//-----------------------------------------------------------------------------
