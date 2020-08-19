#include "ISQueryModelHelper.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
QString ISQueryModelHelper::GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator)
{
	QStringList StringList = MetaForeign->ForeignViewNameField.split(';');
	if (StringList.count() > 1)
	{
		QString SqlText = "concat(";
		for (const QString &String : StringList)
		{
			SqlText += ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + SYMBOL_POINT + MetaTableForeignAlias + '_' + String.toLower() + ", ' ', ";
		}
		SqlText.chop(2);
		SqlText += ')';
		return SqlText;
	}
	return ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + SYMBOL_POINT + MetaTableForeignAlias + '_' + MetaForeign->ForeignViewNameField.toLower();
}
//-----------------------------------------------------------------------------
QString ISQueryModelHelper::GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator)
{
	return TableAlias + QString::number(Iterator);
}
//-----------------------------------------------------------------------------
