#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct ISSYSTEM_EXPORT ISQueryException : ISExceptionBase
{
public:
	ISQueryException(const QString& ExceptionString);
	virtual ~ISQueryException();
};
//-----------------------------------------------------------------------------
