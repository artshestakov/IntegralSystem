#pragma once
#ifndef _ISVARIANT_H_INCLUDED
#define _ISVARIANT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISVariant
{
public:
	ISVariant(ISNamespace::VariantType type = ISNamespace::VariantType::Invalid);
	ISVariant(bool vbool);
	ISVariant(int vint);
	ISVariant(unsigned int vuint);
	ISVariant(double vdouble);
	ISVariant(float vfloat);
	ISVariant(char vchar);
	ISVariant(const std::string &vstring);
	~ISVariant();

	ISNamespace::VariantType GetType() const;

	void SetBool(bool value);
	void SetInt(int value); 
	void SetUInt(unsigned int value);
	void SetDouble(double value);
	void SetFloat(float value);
	void SetChar(char value);
	void SetString(const std::string &value);

private:
	void Clear(ISNamespace::VariantType type);

private:
	ISNamespace::VariantType Type;
	bool VBool;
	int VInt;
	unsigned int VUInt;
	double VDouble;
	float VFloat;
	char VChar;
	std::string VString;
};
//-----------------------------------------------------------------------------
#endif
