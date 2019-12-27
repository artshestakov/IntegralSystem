#include "StdAfx.h"
#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
ISExceptionBase::ISExceptionBase(const QString& ExceptionString)
#ifdef WIN32
	: exception(ExceptionString.toLocal8Bit())
#else
	: exception()
#endif
{
	Description = ExceptionString;
}
//-----------------------------------------------------------------------------
ISExceptionBase::~ISExceptionBase()
{

}
//-----------------------------------------------------------------------------
const char* ISExceptionBase::what() const noexcept
{
	return Description.toStdString().c_str();
}
//-----------------------------------------------------------------------------
QString ISExceptionBase::GetWhat() const
{
	return Description;
}
//-----------------------------------------------------------------------------
