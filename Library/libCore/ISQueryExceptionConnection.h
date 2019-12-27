#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISQueryExceptionConnection : ISQueryException
{
public:
	ISQueryExceptionConnection(const QString& ExceptionString);
	virtual ~ISQueryExceptionConnection();
};
//-----------------------------------------------------------------------------
