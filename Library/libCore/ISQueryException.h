#pragma once
#ifndef _ISQUERYEXCEPTION_H_INCLUDED
#define _ISQUERYEXCEPTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
struct ISQueryException : ISExceptionBase
{
public:
	ISQueryException(const QString& ExceptionString);
	virtual ~ISQueryException();
};
//-----------------------------------------------------------------------------
#endif
