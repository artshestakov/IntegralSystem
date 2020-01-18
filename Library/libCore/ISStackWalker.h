#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "StackWalker.h"
//-----------------------------------------------------------------------------
class ISStackWalker : public StackWalker
{
public:
	ISStackWalker();

	virtual void OnOutput(LPCSTR szText);
	QString GetCallStack() const;

private:
	QString StackString;
};
//-----------------------------------------------------------------------------
