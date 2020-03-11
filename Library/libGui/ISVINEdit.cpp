#include "ISVINEdit.h"
#include "ISConstants.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISVINEdit::ISVINEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_17), this));
	SetPlaceholderText(LANG("Field.Vin.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISVINEdit::~ISVINEdit()
{

}
//-----------------------------------------------------------------------------
