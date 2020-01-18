#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct ISQueryException : ISExceptionBase
{
public:
	ISQueryException(const QString& ExceptionString);
	virtual ~ISQueryException();
};
//-----------------------------------------------------------------------------
