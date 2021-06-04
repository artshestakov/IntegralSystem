#include "ISComboSearchWidgets.h"
#include "ISLocalizationOld.h"
//-----------------------------------------------------------------------------
ISComboSearchBase::ISComboSearchBase(QWidget *parent) : ISComboEdit(parent)
{
    SetEditable(false);
    SetWheelScroll(true);

    AddItem(LANG("Search.Operator.Equally"), static_cast<int>(ISNamespace::SearchType::Equally));
    AddItem(LANG("Search.Operator.NotEqually"), static_cast<int>(ISNamespace::SearchType::NotEqually));
}
//-----------------------------------------------------------------------------
ISComboSearchBase::~ISComboSearchBase()
{

}
//-----------------------------------------------------------------------------
ISNamespace::SearchType ISComboSearchBase::GetOperator()
{
    return static_cast<ISNamespace::SearchType>(GetValue().toInt());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISComboSearchNumber::ISComboSearchNumber(QWidget *parent) : ISComboSearchBase(parent)
{
    AddItem(LANG("Search.Operator.More"), static_cast<int>(ISNamespace::SearchType::More));
    AddItem(LANG("Search.Operator.Less"), static_cast<int>(ISNamespace::SearchType::Less));
    AddItem(LANG("Search.Operator.MoreOrEqual"), static_cast<int>(ISNamespace::SearchType::MoreOrEqual));
    AddItem(LANG("Search.Operator.LessOrEqual"), static_cast<int>(ISNamespace::SearchType::LessOrEqual));
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
    AddItem(LANG("Search.Operator.Begin"), static_cast<int>(ISNamespace::SearchType::Begins));
    AddItem(LANG("Search.Operator.End"), static_cast<int>(ISNamespace::SearchType::Ends));
    AddItem(LANG("Search.Operator.Contains"), static_cast<int>(ISNamespace::SearchType::Contains));
}
//-----------------------------------------------------------------------------
ISComboSearchString::~ISComboSearchString()
{

}
//-----------------------------------------------------------------------------
