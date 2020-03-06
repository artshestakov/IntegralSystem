#include "ISException.h"
//-----------------------------------------------------------------------------
ISExceptionBase::ISExceptionBase(const QString &ExceptionString)
	: std::exception(ExceptionString.toStdString().c_str()),
	Description(ExceptionString)
{
	
}
//-----------------------------------------------------------------------------
ISExceptionBase::~ISExceptionBase()
{

}
//-----------------------------------------------------------------------------
QString ISExceptionBase::What() const
{
	return Description;
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