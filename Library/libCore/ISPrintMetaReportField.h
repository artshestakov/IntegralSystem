#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISPrintMetaReportField : public QObject
{
	Q_OBJECT

public:
	ISPrintMetaReportField(QObject *parent = 0);
	virtual ~ISPrintMetaReportField();

	void SetReplaceValue(const QString &replace_value);
	QString GetReplaceValue() const;

	void SetFieldQuery(const QString &field_query);
	QString GetFieldQuery() const;

	void SetQueryName(const QString &query_name);
	QString GetQueryName() const;

	void SetParameterName(const QString &parameter_name);
	QString GetParameterName() const;

private:
	QString ReplaceValue;
	QString FieldQuery;
	QString QueryName;
	QString ParameterName;
};
//-----------------------------------------------------------------------------
