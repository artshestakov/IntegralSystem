#include "ISQueryException.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryException::ISQueryException(const QString& ExceptionString) : ISExceptionBase(ExceptionString)
{
	ISLOGGER_ERROR(ExceptionString);
}
//-----------------------------------------------------------------------------
ISQueryException::~ISQueryException()
{

}
//-----------------------------------------------------------------------------
