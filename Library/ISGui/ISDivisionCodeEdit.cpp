#include "StdAfx.h"
#include "ISDivisionCodeEdit.h"
//-----------------------------------------------------------------------------
ISDivisionCodeEdit::ISDivisionCodeEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetInputMask("000-000;_");
	connect(GetLineEdit(), &ISQLineEdit::FocusInSignal, this, &ISDivisionCodeEdit::SelectAll);
}
//-----------------------------------------------------------------------------
ISDivisionCodeEdit::~ISDivisionCodeEdit()
{

}
//-----------------------------------------------------------------------------
bool ISDivisionCodeEdit::IsValid() const
{
	if (GetValue().toString().length() == 7)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
