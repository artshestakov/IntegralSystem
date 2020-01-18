#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQueryException.h"
//-----------------------------------------------------------------------------
struct ISQueryExceptionConnection : ISQueryException
{
public:
	ISQueryExceptionConnection(const QString& ExceptionString);
	virtual ~ISQueryExceptionConnection();
};
//-----------------------------------------------------------------------------
