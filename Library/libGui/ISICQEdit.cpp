#include "StdAfx.h"
#include "ISICQEdit.h"
#include "ISDefines.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISICQEdit::ISICQEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_9), this));
	SetPlaceholderText(LANG("Field.Icq.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISICQEdit::~ISICQEdit()
{

}
//-----------------------------------------------------------------------------
