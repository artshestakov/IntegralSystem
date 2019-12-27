#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISExceptionAssert : ISExceptionBase
{
public:
	ISExceptionAssert(const QString& ExceptionString);
	virtual ~ISExceptionAssert();
};
//-----------------------------------------------------------------------------
