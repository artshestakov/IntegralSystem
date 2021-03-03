#include "ISVariant.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISVariant::ISVariant(ISNamespace::VariantType type)
{
	Clear(type);
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(bool value) : ISVariant(ISNamespace::VariantType::Bool)
{
	VBool = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(short value) : ISVariant(ISNamespace::VariantType::Short)
{
	VShort = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(int value) : ISVariant(ISNamespace::VariantType::Int)
{
	VInt = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(ISInt64 value) : ISVariant(ISNamespace::VariantType::Int64)
{
	VInt64 = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(unsigned int value) : ISVariant(ISNamespace::VariantType::UInt)
{
	VUInt = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(double value) : ISVariant(ISNamespace::VariantType::Double)
{
	VDouble = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(float value) : ISVariant(ISNamespace::VariantType::Float)
{
	VFloat = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(char value) : ISVariant(ISNamespace::VariantType::Char)
{
	VChar = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(const std::string &value) : ISVariant(ISNamespace::VariantType::String)
{
	VString = value;
}
//-----------------------------------------------------------------------------
ISVariant::ISVariant(const char *value) : ISVariant(ISNamespace::VariantType::String)
{
	VString = std::string(value);
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
void ISVariant::SetShort(short value)
{
	Clear(ISNamespace::VariantType::Short);
	VShort = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetInt(int value)
{
	Clear(ISNamespace::VariantType::Int);
	VInt = value;
}
//-----------------------------------------------------------------------------
void ISVariant::SetInt64(ISInt64 value)
{
	Clear(ISNamespace::VariantType::Int64);
	VInt64 = value;
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
void ISVariant::SetUuid(const std::string &value)
{
	Clear(ISNamespace::VariantType::Uuid);
	VUuid = value;
}
//-----------------------------------------------------------------------------
std::string ISVariant::ToString()
{
	std::string String;
	switch (Type)
	{
	case ISNamespace::VariantType::Invalid:
		break;

	case ISNamespace::VariantType::Bool:
		String = VBool ? "true" : "false";
		break;

	case ISNamespace::VariantType::Short:
		String = std::to_string(VShort);
		break;

	case ISNamespace::VariantType::Int:
		String = std::to_string(VInt);
		break;

	case ISNamespace::VariantType::Int64:
		String = std::to_string(VInt64);
		break;

	case ISNamespace::VariantType::UInt:
		String = std::to_string(VUInt);
		break;

	case ISNamespace::VariantType::Double:
		String = std::to_string(VDouble);
		break;

	case ISNamespace::VariantType::Float:
		String = std::to_string(VFloat);
		break;

	case ISNamespace::VariantType::Char:
		String.push_back(VChar);
		break;

	case ISNamespace::VariantType::String:
		String = VString;
		break;
	
	case ISNamespace::VariantType::Uuid:
		String = VUuid;
		break;
	}
	return String;
}
//-----------------------------------------------------------------------------
void ISVariant::Clear(ISNamespace::VariantType type)
{
	Type = type;
	VBool = false;
	VShort = 0;
	VInt = 0;
	VInt64 = 0;
	VUInt = 0;
	VDouble = 0;
	VFloat = 0;
	VChar = 0;
	VString.clear();
	VUuid.clear();
}
//-----------------------------------------------------------------------------
