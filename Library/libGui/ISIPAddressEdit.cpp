#include "ISIPAddressEdit.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISIPAddressEdit::ISIPAddressEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_IP_ADDRESS), this));
}
//-----------------------------------------------------------------------------
ISIPAddressEdit::~ISIPAddressEdit()
{

}
//-----------------------------------------------------------------------------
