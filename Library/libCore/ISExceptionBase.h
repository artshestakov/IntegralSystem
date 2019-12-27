#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISExceptionBase : public exception
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
