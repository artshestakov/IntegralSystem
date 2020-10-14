#include "ISTcpAnswer.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer() : ISTcpAnswer(QString())
{
	(*this)["IsError"] = false;
}
//-----------------------------------------------------------------------------
ISTcpAnswer::ISTcpAnswer(const QString &ErrorString) : QVariantMap()
{
	SetError(ErrorString);
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
	if (!ErrorString.isEmpty())
	{
		(*this)["IsError"] = true;
		(*this)["ErrorDescription"] = ErrorString;
	}
}
//-----------------------------------------------------------------------------
