#pragma once
#ifndef _ISTCPQUERY_H_INCLUDED
#define _ISTCPQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpQuery
{
public:
	ISTcpQuery(const QString &query_type);
	~ISTcpQuery();

	QString GetErrorString() const;
	void BindValue(const QString &ParamterName, const QVariant &ParameterValue);
	bool Execute();
	QVariantMap& GetAnswer();

private:
	QString ErrorString;
	QString QueryType;
	QVariantMap Parameters;
	QVariantMap TcpAnswer;
};
//-----------------------------------------------------------------------------
#endif
