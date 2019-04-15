#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct ISSYSTEM_EXPORT ISQueryExceptionConnection : ISQueryException
{
public:
	ISQueryExceptionConnection(const QString& ExceptionString);
	virtual ~ISQueryExceptionConnection();
};
//-----------------------------------------------------------------------------
