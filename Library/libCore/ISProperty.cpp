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
ISProperty& ISProperty::GetInstance()
{
	static ISProperty Properties;
	return Properties;
}
//-----------------------------------------------------------------------------
void ISProperty::SetValue(const QString &PropertyName, const QVariant &Value)
{
	if (Properties.contains(PropertyName))
	{
		Properties[PropertyName] = Value;
	}
	else
	{
		AddProperty(PropertyName, Value);
	}
}
//-----------------------------------------------------------------------------
QVariant ISProperty::GetValue(const QString &PropertyName) const
{
	QVariant Result;
	if (Properties.contains(PropertyName))
	{
		Result = Properties.value(PropertyName);
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISProperty::GetValueBool(const QString &PropertyName) const
{
	return GetValue(PropertyName).toBool();
}
//-----------------------------------------------------------------------------
void ISProperty::AddProperty(const QString &PropertyName, const QVariant &Value)
{
	Properties.insert(PropertyName, Value);
}
//-----------------------------------------------------------------------------
