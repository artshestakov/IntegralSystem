#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryPoolObject
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
