#include "ISTcpAnswer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer() : QVariantMap()
{
	insert("IsError", false);
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
	insert("ErrorDescription", ErrorString);
}
//-----------------------------------------------------------------------------
