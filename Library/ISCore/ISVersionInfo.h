#pragma once
#ifndef _ISVERSIONINFO_H_INCLUDED
#define _ISVERSIONINFO_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISVersionInfo
{
public:
	ISBuildInfoStruct Info; //Информация о версии

public:
	static ISVersionInfo& Instance();

private:
	ISVersionInfo();
	~ISVersionInfo();
	ISVersionInfo(ISVersionInfo const &) {};
	ISVersionInfo& operator=(ISVersionInfo const&) { return *this; };
};
//-----------------------------------------------------------------------------
#endif
