#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct ISQueryExceptionSyntax : ISQueryException
{
public:
	ISQueryExceptionSyntax(const QString& ExceptionString);
	virtual ~ISQueryExceptionSyntax();
};
//-----------------------------------------------------------------------------
