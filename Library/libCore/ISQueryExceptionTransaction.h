#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISQueryExceptionTransaction : ISQueryException
{
public:
	ISQueryExceptionTransaction(const QString& ExceptionString);
	virtual ~ISQueryExceptionTransaction();
};
//-----------------------------------------------------------------------------
