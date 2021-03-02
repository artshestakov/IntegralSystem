#pragma once
#ifndef _ISUUID_H_INCLUDED
#define _ISUUID_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISUuid
{
public:
	ISUuid(const char *Char);
	ISUuid();
	~ISUuid();

	std::string ToString() const;
	void Clear();

private:
	std::string String;
};
//-----------------------------------------------------------------------------
#endif
