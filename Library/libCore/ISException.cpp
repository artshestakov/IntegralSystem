#include "ISException.h"
//-----------------------------------------------------------------------------
ISExceptionBase::ISExceptionBase(const QString &ExceptionString)
	: std::exception(ExceptionString.toStdString().c_str())
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExceptionConnectionDB::ISExceptionConnectionDB(const QString &ExceptionString)
	: ISExceptionBase(ExceptionString)
{

}
//-----------------------------------------------------------------------------
ISExceptionConnectionDB::~ISExceptionConnectionDB()
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISExceptionSqlSyntax::ISExceptionSqlSyntax(const QString &ExceptionString)
	: ISExceptionBase(ExceptionString)
{

}
//-----------------------------------------------------------------------------
ISExceptionSqlSyntax::~ISExceptionSqlSyntax()
{

}
//-----------------------------------------------------------------------------