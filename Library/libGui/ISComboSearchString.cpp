#include "StdAfx.h"
#include "ISComboSearchString.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboSearchString::ISComboSearchString(QWidget *parent) : ISComboSearchBase(parent)
{
	AddItem(LOCALIZATION("Search.Operator.Begin"), ISNamespace::SOT_Begins);
	AddItem(LOCALIZATION("Search.Operator.End"), ISNamespace::SOT_Ends);
	AddItem(LOCALIZATION("Search.Operator.Contains"), ISNamespace::SOT_Contains);
}
//-----------------------------------------------------------------------------
ISComboSearchString::~ISComboSearchString()
{

}
//-----------------------------------------------------------------------------
