#include "ISTcpQueryAuth.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISTcpQueryAuth::ISTcpQueryAuth(const QString &Login, const QString &Password) : ISTcpQuery(API_AUTH)
{
	SetDecryptAnswer(false);
	BindValue("Login", Login);
	BindValue("Password", Password);
}
//-----------------------------------------------------------------------------
ISTcpQueryAuth::~ISTcpQueryAuth()
{

}
//-----------------------------------------------------------------------------
