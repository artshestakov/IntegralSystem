#include "ISTypes.h"
//-----------------------------------------------------------------------------
ISRangeStruct::ISRangeStruct() : BeginValue(QVariant()), EndValue(QVariant())
{

}
//-----------------------------------------------------------------------------
ISRangeStruct::ISRangeStruct(const QVariant &begin_value, const QVariant &end_value) : BeginValue(begin_value), EndValue(end_value)
{

}
//-----------------------------------------------------------------------------
void ISRangeStruct::Clear()
{
	BeginValue.clear();
	EndValue.clear();
}
//-----------------------------------------------------------------------------
bool ISRangeStruct::IsValid() const
{
	return BeginValue.isValid() && EndValue.isValid();
}
//-----------------------------------------------------------------------------
