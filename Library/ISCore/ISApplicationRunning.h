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

private:
	std::string Key;
};
//-----------------------------------------------------------------------------
