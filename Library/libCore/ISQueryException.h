#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISQueryException : ISExceptionBase
{
public:
	ISQueryException(const QString& ExceptionString);
	virtual ~ISQueryException();
};
//-----------------------------------------------------------------------------
