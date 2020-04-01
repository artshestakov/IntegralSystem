#include "ISProperty.h"
//-----------------------------------------------------------------------------
ISProperty::ISProperty()
{

}
//-----------------------------------------------------------------------------
ISProperty::~ISProperty()
{

}
//-----------------------------------------------------------------------------
ISProperty& ISProperty::Instance()
{
	static ISProperty Properties;
	return Properties;
}
//-----------------------------------------------------------------------------
void ISProperty::SetValue(const QString &PropertyName, const QVariant &Value)
{
	Map[PropertyName] = Value;
}
//-----------------------------------------------------------------------------
QVariant ISProperty::GetValue(const QString &PropertyName) const
{
	QVariant Value;
	if (Map.count(PropertyName))
	{
		Value = Map.at(PropertyName);
	}
	return Value;
}
//-----------------------------------------------------------------------------
