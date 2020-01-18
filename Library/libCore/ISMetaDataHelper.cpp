#include "ISMetaDataHelper.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
QString ISMetaDataHelper::GenerateSqlQueryFromForeign(PMetaClassForeign *MetaForeign, const QString &SqlFilter, const QVariant &ObjectID)
{
	PMetaClassTable *MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->GetForeignClass()); //Таблица на которую ссылается внешний ключ
	QString ForeignAlias = MetaTableForeign->GetAlias();
	QStringList FieldList = MetaForeign->GetForeignViewNameField().split(";");

	QString SqlQuery = "SELECT " + ForeignAlias + "_" + MetaForeign->GetForeginField().toLower() + " AS ID, concat(";

	for (int i = 0; i < FieldList.count(); i++) //Обход полей (которые должны быть отображены)
	{
		SqlQuery += ForeignAlias + "_" + FieldList.at(i).toLower() + ", ' ', ";
	}

	ISSystem::RemoveLastSymbolFromString(SqlQuery, 7);

	SqlQuery += ") ";
	SqlQuery += "AS Value \n";
	SqlQuery += "FROM " + MetaTableForeign->GetName() + " \n";
	SqlQuery += "WHERE NOT " + ForeignAlias + "_isdeleted \n";

	if (SqlFilter.length())
	{
		SqlQuery += "AND " + SqlFilter + " \n";
	}

	if (ObjectID.isValid())
	{
		SqlQuery += "AND " + ForeignAlias + "_" + MetaForeign->GetForeginField() + " = :ObjectID \n";
	}

	SqlQuery += "ORDER BY ";

	if (MetaForeign->GetOrderField().length()) //Если указано поле для сортировки
	{
		SqlQuery += MetaTableForeign->GetAlias() + "_" + MetaForeign->GetOrderField() + " ASC";
	}
	else
	{
		SqlQuery += "2 ASC";
	}

	return SqlQuery;
}
//-----------------------------------------------------------------------------
