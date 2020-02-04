#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISQueryExceptionConnection : public ISQueryException
{
public:
	ISQueryExceptionConnection(const QString& ExceptionString);
	virtual ~ISQueryExceptionConnection();
};
//-----------------------------------------------------------------------------
