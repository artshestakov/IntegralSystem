#include "ISExceptionBase.h"
//-----------------------------------------------------------------------------
ISExceptionBase::ISExceptionBase(const QString& ExceptionString)
#ifdef WIN32
	: std::exception(ExceptionString.toLocal8Bit())
#else
	: std::exception()
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
