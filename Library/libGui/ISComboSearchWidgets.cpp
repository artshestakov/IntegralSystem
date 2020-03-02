#include "ISComboSearchWidgets.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboSearchBase::ISComboSearchBase(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);
	SetWheelScroll(true);

	AddItem(LANG("Search.Operator.Equally"), ISNamespace::SOT_Equally);
	AddItem(LANG("Search.Operator.NotEqually"), ISNamespace::SOT_NotEqually);
}
//-----------------------------------------------------------------------------
ISComboSearchBase::~ISComboSearchBase()
{

}
//-----------------------------------------------------------------------------
ISNamespace::SearchOperatorType ISComboSearchBase::GetOperator()
{
	return static_cast<ISNamespace::SearchOperatorType>(GetValue().toInt());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
