#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct ISQueryExceptionTransaction : ISQueryException
{
public:
	ISQueryExceptionTransaction(const QString& ExceptionString);
	virtual ~ISQueryExceptionTransaction();
};
//-----------------------------------------------------------------------------
