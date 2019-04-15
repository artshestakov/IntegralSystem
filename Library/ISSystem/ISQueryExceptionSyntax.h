#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct ISSYSTEM_EXPORT ISQueryExceptionSyntax : ISQueryException
{
public:
	ISQueryExceptionSyntax(const QString& ExceptionString);
	virtual ~ISQueryExceptionSyntax();
};
//-----------------------------------------------------------------------------
