#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISQueryExceptionSyntax : ISQueryException
{
public:
	ISQueryExceptionSyntax(const QString& ExceptionString);
	virtual ~ISQueryExceptionSyntax();
};
//-----------------------------------------------------------------------------
