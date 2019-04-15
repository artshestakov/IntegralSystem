#include "StdAfx.h"
#include "ISQueryException.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISQueryException::ISQueryException(const QString& ExceptionString) : ISExceptionBase(ExceptionString)
{
	ISDebug::ShowExceptionString(QString("SQL-EXCEPTION. %1").arg(ExceptionString));
}
//-----------------------------------------------------------------------------
ISQueryException::~ISQueryException()
{

}
//-----------------------------------------------------------------------------
