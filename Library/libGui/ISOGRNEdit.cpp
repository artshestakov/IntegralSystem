#include "ISOGRNEdit.h"
#include "ISLocalization.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISOGRNEdit::ISOGRNEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_13), this));
	SetPlaceholderText(LANG("Field.Ogrn.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISOGRNEdit::~ISOGRNEdit()
{

}
//-----------------------------------------------------------------------------
