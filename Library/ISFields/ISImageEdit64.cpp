#include "StdAfx.h"
#include "ISImageEdit64.h"
//-----------------------------------------------------------------------------
ISImageEdit64::ISImageEdit64(QWidget *parent) : ISImageEdit(parent)
{
	
}
//-----------------------------------------------------------------------------
ISImageEdit64::~ISImageEdit64()
{

}
//-----------------------------------------------------------------------------
void ISImageEdit64::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		ISImageEdit::SetValue(QByteArray::fromBase64(value.toString().toUtf8()));
	}
}
//-----------------------------------------------------------------------------
QVariant ISImageEdit64::GetValue() const
{
	QVariant Value = ISImageEdit::GetValue();
	if (Value.isValid())
	{
		return QString(Value.toByteArray().toBase64()).toUtf8();
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
