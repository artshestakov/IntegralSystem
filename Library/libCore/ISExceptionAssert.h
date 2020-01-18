#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct ISExceptionAssert : ISExceptionBase
{
public:
	ISExceptionAssert(const QString& ExceptionString);
	virtual ~ISExceptionAssert();
};
//-----------------------------------------------------------------------------
