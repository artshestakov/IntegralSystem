#include "StdAfx.h"
#include "ISComboSearchBase.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISComboSearchBase::ISComboSearchBase(QWidget *parent) : ISComboEdit(parent)
{
	SetEditable(false);
	SetWheelScroll(true);

	AddItem(LOCALIZATION("Search.Operator.Equally"), ISNamespace::SOT_Equally);
	AddItem(LOCALIZATION("Search.Operator.NotEqually"), ISNamespace::SOT_NotEqually);
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
