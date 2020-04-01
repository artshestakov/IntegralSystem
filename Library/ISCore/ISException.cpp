#include "ISException.h"
//-----------------------------------------------------------------------------
ISExceptionBase::ISExceptionBase(const QString &ExceptionString)
#ifdef WIN32
	: std::exception(ExceptionString.toStdString().c_str()),
#else
        : std::exception(),
#endif
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
