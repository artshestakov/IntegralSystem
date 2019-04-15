#include "StdAfx.h"
#include "ISObjectModelItem.h"
//-----------------------------------------------------------------------------
ISObjectModelItem::ISObjectModelItem(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISObjectModelItem::~ISObjectModelItem()
{

}
//-----------------------------------------------------------------------------
void ISObjectModelItem::SetOldValue(const QVariant &old_value)
{
	OldValue = old_value;
}
//-----------------------------------------------------------------------------
QVariant ISObjectModelItem::GetOldValue() const
{
	return OldValue;
}
//-----------------------------------------------------------------------------
void ISObjectModelItem::SetNewValue(const QVariant &new_value)
{
	NewValue = new_value;
}
//-----------------------------------------------------------------------------
QVariant ISObjectModelItem::GetNewValue() const
{
	return NewValue;
}
//-----------------------------------------------------------------------------
