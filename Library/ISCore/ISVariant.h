#pragma once
#ifndef _ISVARIANT_H_INCLUDED
#define _ISVARIANT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISVariant
{
public:
	ISVariant(ISNamespace::VariantType type = ISNamespace::VariantType::Invalid);
	ISVariant(bool value);
	ISVariant(short value);
	ISVariant(int value);
	ISVariant(__int64 value);
	ISVariant(unsigned int value);
	ISVariant(double value);
	ISVariant(float value);
	ISVariant(char value);
	ISVariant(const std::string &value);
	ISVariant(const char *value);
	ISVariant(const ISUuid &value);
	~ISVariant();

	ISNamespace::VariantType GetType() const;

	void SetBool(bool value);
	void SetShort(short value);
	void SetInt(int value);
	void SetInt64(__int64 value);
	void SetUInt(unsigned int value);
	void SetDouble(double value);
	void SetFloat(float value);
	void SetChar(char value);
	void SetString(const std::string &value);
	void SetUuid(const ISUuid &value);

	std::string ToString(); //Конвертация значения в строку

private:
	void Clear(ISNamespace::VariantType type);

private:
	ISNamespace::VariantType Type;
	bool VBool;
	short VShort;
	int VInt;
	__int64 VInt64;
	unsigned int VUInt;
	double VDouble;
	float VFloat;
	char VChar;
	std::string VString;
	ISUuid VUuid;
};
//-----------------------------------------------------------------------------
#endif
