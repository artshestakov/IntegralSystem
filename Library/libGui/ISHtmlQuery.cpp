#include "ISHtmlQuery.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISHtmlQuery::ISHtmlQuery(const QString &sql_query_text, const QString &table_html_name)
{
	TableHtmlName = table_html_name;
	SqlQueryText = sql_query_text;
	qSelect = ISQuery(SqlQueryText);
}
//-----------------------------------------------------------------------------
ISHtmlQuery::ISHtmlQuery(const ISQuery &Query, const QString &table_html_name)
{
	TableHtmlName = table_html_name;
	qSelect = Query;
}
//-----------------------------------------------------------------------------
ISHtmlQuery::~ISHtmlQuery()
{

}
//-----------------------------------------------------------------------------
QString ISHtmlQuery::GetTableHtmlName() const
{
	return TableHtmlName;
}
//-----------------------------------------------------------------------------
void ISHtmlQuery::BindValue(const QString &Parameter, const QVariant &Value)
{
	bool BindValue = qSelect.BindValue(Parameter, Value);
	IS_ASSERT(BindValue, "BindValue false from parameter '" + Parameter + '\'');
}
//-----------------------------------------------------------------------------
void ISHtmlQuery::Replace(const QString &AsField, const QString &ReplaceText)
{
	bool Contains = SqlQueryText.contains(AsField);
	IS_ASSERT(Contains, "");
	ReplaceFields.insert(AsField, ReplaceText);
}
//-----------------------------------------------------------------------------
QString ISHtmlQuery::GetHtmlTableQuery() const
{
	QString TableHtml = "<table border=\"1\">\n";

	QSqlQueryModel SqlQueryModel;
	SqlQueryModel.setQuery(qSelect.GetSqlQuery());

	if (SqlQueryModel.columnCount())
	{
		TableHtml += "<tr>";
		for (int Column = 0; Column < SqlQueryModel.columnCount(); ++Column)
		{
			QString HeaderData = SqlQueryModel.headerData(Column, Qt::Horizontal).toString();

			TableHtml += "<th>" + HeaderData + "</th>";
		}
		TableHtml += "</tr>\n";

		for (int Row = 0; Row < SqlQueryModel.rowCount(); ++Row)
		{
			TableHtml += "<tr>";
			for (int Column = 0; Column < SqlQueryModel.columnCount(); ++Column)
			{
				QModelIndex ModelIndex = SqlQueryModel.index(Row, Column);
				QString StringValue = ModelIndex.data().toString();
				TableHtml += "<th>" + StringValue + "</th>";
			}
			TableHtml += "</tr>\n";
		}
	}

	TableHtml += "</table>";

	return TableHtml;
}
//-----------------------------------------------------------------------------
