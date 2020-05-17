#pragma once
#ifndef _ISTCPQUERYAUTH_H_INCLUDED
#define _ISTCPQUERYAUTH_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpQueryAuth : public ISTcpQuery
{
public:
	ISTcpQueryAuth(const QString &Login, const QString &Password);
	~ISTcpQueryAuth();
};
//-----------------------------------------------------------------------------
#endif
