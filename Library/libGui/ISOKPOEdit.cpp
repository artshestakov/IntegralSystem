#include "StdAfx.h"
#include "ISOKPOEdit.h"
#include "ISLocalization.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISOKPOEdit::ISOKPOEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetFixedWidth(180);
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_NUMBER_COUNT_8), this));
	SetPlaceholderText(LANG("Field.Okpo.PlaceholderText"));
}
//-----------------------------------------------------------------------------
ISOKPOEdit::~ISOKPOEdit()
{

}
//-----------------------------------------------------------------------------
