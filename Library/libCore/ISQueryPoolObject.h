#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISQueryPoolObject
{
public:
	ISQueryPoolObject(const QString &sql_text);
	virtual ~ISQueryPoolObject();

	QString GetSqlText() const;
	void AddBindValue(const QString &Parameter, const QVariant &Value);
	QVariantMap GetParameters() const;

private:
	QString SqlText;
	QVariantMap Parameters;
};
//-----------------------------------------------------------------------------
