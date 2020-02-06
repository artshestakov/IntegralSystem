#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQuery.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISHtmlQuery
{
public:
	ISHtmlQuery(const QString &sql_query_text, const QString &table_html_name);
	ISHtmlQuery(const ISQuery &Query, const QString &table_html_name);
	virtual ~ISHtmlQuery();

	QString GetTableHtmlName() const;

	void BindValue(const QString &Parameter, const QVariant &Value);
	void Replace(const QString &AsField, const QString &ReplaceText);
	QString GetHtmlTableQuery() const;

private:
	QString TableHtmlName;
	QString SqlQueryText;
	ISQuery qSelect;
	QStringMap ReplaceFields;
};
//-----------------------------------------------------------------------------
