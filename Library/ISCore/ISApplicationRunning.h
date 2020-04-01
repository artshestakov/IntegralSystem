#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISApplicationRunning
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
