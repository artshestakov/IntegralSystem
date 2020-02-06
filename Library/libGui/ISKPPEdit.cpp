#include "ISKPPEdit.h"
#include "ISConstants.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISKPPEdit::ISKPPEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_9), this));
	SetPlaceholderText(LANG("Field.Kpp.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISKPPEdit::~ISKPPEdit()
{

}
//-----------------------------------------------------------------------------
