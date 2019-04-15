#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct ISSYSTEM_EXPORT ISExceptionAssert : ISExceptionBase
{
public:
	ISExceptionAssert(const QString& ExceptionString);
	virtual ~ISExceptionAssert();
};
//-----------------------------------------------------------------------------
