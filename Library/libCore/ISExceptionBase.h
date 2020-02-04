#pragma once
#ifndef _ISEXCEPTIONBASE_H_INCLUDED
#define _ISEXCEPTIONBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISExceptionBase : public std::exception
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
#endif
