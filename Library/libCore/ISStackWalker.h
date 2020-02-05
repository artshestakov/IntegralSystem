#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "StackWalker.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISStackWalker : public StackWalker
{
public:
	ISStackWalker();

	virtual void OnOutput(LPCSTR Data);
	std::string GetCallStack() const;

private:
	std::string String;
};
//-----------------------------------------------------------------------------
