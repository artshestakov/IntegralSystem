#include "StdAfx.h"
#include "ISTaskImportantEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISTaskImportantEdit::ISTaskImportantEdit(QWidget *parent) : ISCheckEdit(parent)
{
	GetCheckBox()->setIcon(BUFFER_ICONS("Task.Important.Unchecked"));
	GetCheckBox()->setToolTip(LANG("ISImportantEditToolTip"));
	connect(this, &ISTaskImportantEdit::ValueChange, this, &ISTaskImportantEdit::ImportantChanged);
}
//-----------------------------------------------------------------------------
ISTaskImportantEdit::~ISTaskImportantEdit()
{

}
//-----------------------------------------------------------------------------
void ISTaskImportantEdit::ImportantChanged(const QVariant &value)
{
	if (value.toBool())
	{
		GetCheckBox()->setIcon(BUFFER_ICONS("Task.Important.Checked"));
	}
	else
	{
		GetCheckBox()->setIcon(BUFFER_ICONS("Task.Important.Unchecked"));
	}
}
//-----------------------------------------------------------------------------
