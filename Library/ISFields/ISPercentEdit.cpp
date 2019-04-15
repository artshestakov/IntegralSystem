#include "StdAfx.h"
#include "ISPercentEdit.h"
//-----------------------------------------------------------------------------
ISPercentEdit::ISPercentEdit(QWidget *parent) : ISDoubleEdit(parent)
{
	SetMinimum(0);
	SetMaximum(100);
	SetSuffix("%");
}
//-----------------------------------------------------------------------------
ISPercentEdit::~ISPercentEdit()
{

}
//-----------------------------------------------------------------------------
