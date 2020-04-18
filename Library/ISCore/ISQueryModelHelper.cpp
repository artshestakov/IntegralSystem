#include "ISQueryModelHelper.h"
#include "ISSystem.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
QString ISQueryModelHelper::GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, int Iterator)
{
	QStringList StringList = MetaForeign->ForeignViewNameField.split(';');
	if (StringList.count() > 1)
	{
		QString SqlText = "concat(";
		for (int i = 0; i < StringList.count(); ++i)
		{
			SqlText += ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + SYMBOL_POINT + MetaTableForeignAlias + '_' + StringList.at(i).toLower() + ", ' ', ";
			if (i == StringList.count() - 1)
			{
				continue;
			}
		}
		SqlText.chop(2);
		SqlText += ')';
		return SqlText;
	}
	return ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + SYMBOL_POINT + MetaTableForeignAlias + '_' + MetaForeign->ForeignViewNameField.toLower();
}
//-----------------------------------------------------------------------------
QString ISQueryModelHelper::GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator)
{
	return TableAlias + QString::number(Iterator);
}
//-----------------------------------------------------------------------------
