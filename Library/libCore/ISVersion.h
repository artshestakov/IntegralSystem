#pragma once
#ifndef _ISVERSION_H_INCLUDED
#define _ISVERSION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISVersion
{
public:
	static ISVersion& Instance();
	QString ToString() const;
	ISVersionStruct Info;

private:
	ISVersion();
	~ISVersion();
	ISVersion(ISVersion const &) {};
	ISVersion& operator=(ISVersion const&) { return *this; };
};
//-----------------------------------------------------------------------------
#endif
