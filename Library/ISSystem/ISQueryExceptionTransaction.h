#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct ISSYSTEM_EXPORT ISQueryExceptionTransaction : ISQueryException
{
public:
	ISQueryExceptionTransaction(const QString& ExceptionString);
	virtual ~ISQueryExceptionTransaction();
};
//-----------------------------------------------------------------------------
