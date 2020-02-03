#include "ISBIKEdit.h"
#include "ISDefines.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISBIKEdit::ISBIKEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_9), this));
	SetPlaceholderText(LANG("Field.Bik.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISBIKEdit::~ISBIKEdit()
{

}
//-----------------------------------------------------------------------------
