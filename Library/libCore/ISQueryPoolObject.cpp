#include "ISQueryPoolObject.h"
//-----------------------------------------------------------------------------
ISQueryPoolObject::ISQueryPoolObject(const QString &sql_text)
{
	SqlText = sql_text;
}
//-----------------------------------------------------------------------------
ISQueryPoolObject::~ISQueryPoolObject()
{

}
//-----------------------------------------------------------------------------
QString ISQueryPoolObject::GetSqlText() const
{
	return SqlText;
}
//-----------------------------------------------------------------------------
void ISQueryPoolObject::AddBindValue(const QString &Parameter, const QVariant &Value)
{
	Parameters.insert(Parameter, Value);
}
//-----------------------------------------------------------------------------
QVariantMap ISQueryPoolObject::GetParameters() const
{
	return Parameters;
}
//-----------------------------------------------------------------------------
