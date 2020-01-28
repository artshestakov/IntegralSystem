#include "ISQueryModelHelper.h"
#include "ISDefines.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
QString ISQueryModelHelper::GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaClassForeign *MetaForeign, int Iterator)
{
	QString ForeignViewNameField = MetaForeign->GetForeignViewNameField();
	QStringList StringList = ForeignViewNameField.split(";");
	
	if (StringList.count() > 1)
	{
		QString SqlText = "concat(";
		for (int i = 0; i < StringList.count(); ++i)
		{
			SqlText += ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + "." + MetaTableForeignAlias + "_" + StringList.at(i).toLower() + ", ' ', ";
			if (i == StringList.count() - 1)
			{
				continue;
			}
		}

		ISSystem::RemoveLastSymbolFromString(SqlText, 2);
		SqlText += ")";
		return SqlText;
	}

	return ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + "." + MetaTableForeignAlias + "_" + ForeignViewNameField.toLower();
}
//-----------------------------------------------------------------------------
QString ISQueryModelHelper::GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator)
{
	QString Alias = TableAlias + QString::number(Iterator);
	return Alias;
}
//-----------------------------------------------------------------------------
