#pragma once
#ifndef _ISTCPANSWER_H_INCLUDED
#define _ISTCPANSWER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISTcpAnswer : public QVariantMap
{
public:
	ISTcpAnswer();
	ISTcpAnswer(const QString &ErrorString);
	~ISTcpAnswer();

	bool IsError();
	QString GetErrorString();
	void SetError(const QString &ErrorString);
};
//-----------------------------------------------------------------------------
#endif
