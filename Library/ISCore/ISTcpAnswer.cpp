#include "ISTcpAnswer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer() : QVariantMap()
{
	insert("IsError", false);
	insert("ErrorDescription", NO_ERROR_STRING);
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
