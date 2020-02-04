#pragma once
#ifndef _ISQUERYEXCEPTION_H_INCLUDED
#define _ISQUERYEXCEPTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISQueryException : public ISExceptionBase
{
public:
	ISQueryException(const QString& ExceptionString);
	virtual ~ISQueryException();
};
//-----------------------------------------------------------------------------
#endif
