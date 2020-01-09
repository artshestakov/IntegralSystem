#include "StdAfx.h"
#include "ISComboSearchNumber.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboSearchNumber::ISComboSearchNumber(QWidget *parent) : ISComboSearchBase(parent)
{
	AddItem(LOCALIZATION("Search.Operator.More"), ISNamespace::SOT_More);
	AddItem(LOCALIZATION("Search.Operator.Less"), ISNamespace::SOT_Less);
	AddItem(LOCALIZATION("Search.Operator.MoreOrEqual"), ISNamespace::SOT_MoreOrEqual);
	AddItem(LOCALIZATION("Search.Operator.LessOrEqual"), ISNamespace::SOT_LessOrEqual);
}
//-----------------------------------------------------------------------------
ISComboSearchNumber::~ISComboSearchNumber()
{

}
//-----------------------------------------------------------------------------
