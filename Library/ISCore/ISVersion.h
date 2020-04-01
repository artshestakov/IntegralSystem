#pragma once
#ifndef _ISVERSION_H_INCLUDED
#define _ISVERSION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISVersion
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
