#include "ISTcpAnswer.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer() : QVariantMap()
{
	insert("IsError", false);
	insert("ErrorDescription", QString());
}
//-----------------------------------------------------------------------------
ISTcpAnswer::~ISTcpAnswer()
{

}
//-----------------------------------------------------------------------------
bool ISTcpAnswer::IsError()
{
	return (*this)["IsError"].toBool();
}
//-----------------------------------------------------------------------------
QString ISTcpAnswer::GetErrorString()
{
	return (*this)["ErrorDescription"].toString();
}
//-----------------------------------------------------------------------------
void ISTcpAnswer::SetError(const QString &ErrorString)
{
	(*this)["IsError"] = true;
	(*this)["ErrorDescription"] = ErrorString;
}
//-----------------------------------------------------------------------------
