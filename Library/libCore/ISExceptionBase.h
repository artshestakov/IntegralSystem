#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct ISExceptionBase : public std::exception
{
public:
	ISExceptionBase(const QString& ExceptionString);
	virtual ~ISExceptionBase();

    virtual const char *what() const noexcept;
	QString GetWhat() const;

private:
	QString Description;
};
//-----------------------------------------------------------------------------
