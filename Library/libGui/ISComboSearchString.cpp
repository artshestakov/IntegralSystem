#include "StdAfx.h"
#include "ISComboSearchString.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboSearchString::ISComboSearchString(QWidget *parent) : ISComboSearchBase(parent)
{
	AddItem(LANG("Search.Operator.Begin"), ISNamespace::SOT_Begins);
	AddItem(LANG("Search.Operator.End"), ISNamespace::SOT_Ends);
	AddItem(LANG("Search.Operator.Contains"), ISNamespace::SOT_Contains);
}
//-----------------------------------------------------------------------------
ISComboSearchString::~ISComboSearchString()
{

}
//-----------------------------------------------------------------------------
