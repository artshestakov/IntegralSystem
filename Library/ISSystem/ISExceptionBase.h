#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
struct ISSYSTEM_EXPORT ISExceptionBase : public exception
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
