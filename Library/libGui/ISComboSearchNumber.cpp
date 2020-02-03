#include "ISComboSearchNumber.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboSearchNumber::ISComboSearchNumber(QWidget *parent) : ISComboSearchBase(parent)
{
	AddItem(LANG("Search.Operator.More"), ISNamespace::SOT_More);
	AddItem(LANG("Search.Operator.Less"), ISNamespace::SOT_Less);
	AddItem(LANG("Search.Operator.MoreOrEqual"), ISNamespace::SOT_MoreOrEqual);
	AddItem(LANG("Search.Operator.LessOrEqual"), ISNamespace::SOT_LessOrEqual);
}
//-----------------------------------------------------------------------------
ISComboSearchNumber::~ISComboSearchNumber()
{

}
//-----------------------------------------------------------------------------
