#include "ISQueryException.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISQueryException::ISQueryException(const QString& ExceptionString) : ISExceptionBase(ExceptionString)
{
	ISDebug::ShowErrorString(ExceptionString);
}
//-----------------------------------------------------------------------------
ISQueryException::~ISQueryException()
{

}
//-----------------------------------------------------------------------------
