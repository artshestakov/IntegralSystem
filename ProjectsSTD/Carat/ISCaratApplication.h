#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCaratApplication
{
public:
	ISCaratApplication(int argc, char **argv);
	~ISCaratApplication();

	std::string GetErrorString() const;

	bool Init();
	int Start();

private:
	void ShutdownController();

private:
	std::string ErrorString;
	bool IsRunning;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
