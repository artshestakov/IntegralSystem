#include "ISVariant.h"
//-----------------------------------------------------------------------------
ISVariant::ISVariant(ISNamespace::VariantType type)
{
	Clear(type);
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(bool vbool)
	: ISVariant(ISNamespace::VariantType::Bool)
{
	VBool = vbool;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(int vint)
	: ISVariant(ISNamespace::VariantType::Int)
{
	VInt = vint;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(unsigned int vuint)
	: ISVariant(ISNamespace::VariantType::UInt)
{
	VUInt = vuint;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(double vdouble)
	: ISVariant(ISNamespace::VariantType::Double)
{
	VDouble = vdouble;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(float vfloat)
	: ISVariant(ISNamespace::VariantType::Float)
{
	VFloat = vfloat;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(char vchar)
	: ISVariant(ISNamespace::VariantType::Char)
{
	VChar = vchar;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(const std::string &vstring)
	: ISVariant(ISNamespace::VariantType::String)
{
	VString = vstring;
}
//-----------------------------------------------------------------------------
ISVariant::~ISVariant()
{

}
//-----------------------------------------------------------------------------
ISNamespace::VariantType ISVariant::GetType() const
{
	return Type;
}
//-----------------------------------------------------------------------------
void ISVariant::SetBool(bool value)
{
	Clear(ISNamespace::VariantType::Bool);
	VBool = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetInt(int value)
{
	Clear(ISNamespace::VariantType::Int);
	VInt = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetUInt(unsigned int value)
{
	Clear(ISNamespace::VariantType::UInt);
	VUInt = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetDouble(double value)
{
	Clear(ISNamespace::VariantType::Double);
	VDouble = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetFloat(float value)
{
	Clear(ISNamespace::VariantType::Float);
	VFloat = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetChar(char value)
{
	Clear(ISNamespace::VariantType::Char);
	VChar = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetString(const std::string &value)
{
	Clear(ISNamespace::VariantType::String);
	VString = value;
}
//-----------------------------------------------------------------------------
void ISVariant::Clear(ISNamespace::VariantType type)
{
	Type = type;
	VBool = false;
	VInt = 0;
	VUInt = 0;
	VDouble = 0;
	VFloat = 0;
	VChar = 0;
	VString.clear();
}
//-----------------------------------------------------------------------------
