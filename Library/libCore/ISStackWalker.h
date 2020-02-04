#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "StackWalker.h"
//-----------------------------------------------------------------------------
class ISStackWalker : public StackWalker
{
public:
	ISStackWalker();

	virtual void OnOutput(LPCSTR Data);
	std::string GetCallStack() const;

private:
	std::string String;
};
//-----------------------------------------------------------------------------
