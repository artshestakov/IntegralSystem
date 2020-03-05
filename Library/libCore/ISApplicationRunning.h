#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISApplicationRunning
{
public:
	ISApplicationRunning(const std::string &key);
	~ISApplicationRunning();

	bool IsRunning();
	bool IsAnotherRunning();
	bool TryToRun();
	void Release();

private:
	std::string Key;
};
//-----------------------------------------------------------------------------
