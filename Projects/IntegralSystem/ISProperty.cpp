#include "ISProperty.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISProperty::ISProperty()
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISProperty::~ISProperty()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
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
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Map[PropertyName] = Value;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
QVariant ISProperty::GetValue(const QString &PropertyName)
{
	QVariant Value;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (Map.count(PropertyName))
	{
		Value = Map.at(PropertyName);
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Value;
}
//-----------------------------------------------------------------------------
