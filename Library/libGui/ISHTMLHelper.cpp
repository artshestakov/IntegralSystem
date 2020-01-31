#include "ISHTMLHelper.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
QString ISHTMLHelper::GenerateTableFromQuerySelect(const QString &QueryText)
{
	QString Table = QString();
	ISQuery qSelect(QueryText);
	return Table;
}
//-----------------------------------------------------------------------------
QString ISHTMLHelper::GenerateTableFromQuerySelect(ISQuery Query)
{
	QString Table = QString();
	
	QSqlQueryModel Model;
	Model.setQuery(Query.GetSqlQuery());

	for (int Row = 0; Row < Model.rowCount(); Row++)
	{
		Table += "<tr>";

		for (int Column = 0; Column < Model.columnCount(); Column++)
		{
			QModelIndex ModelIndex = Model.index(Row, Column);
			QString Value = ModelIndex.data().toString();

			Table += "<th>" + Value + "</th>";
		}

		Table += "</tr>\n";
	}

	return Table;
}
//-----------------------------------------------------------------------------
