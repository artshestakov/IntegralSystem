#include "ISMetaDataHelper.h"
#include "ISMetaData.h"
#include "ISSystem.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_CONFIGURATION = PREPARE_QUERY("SELECT get_configuration_name()");
//-----------------------------------------------------------------------------
QString ISMetaDataHelper::GenerateSqlQueryFromForeign(PMetaForeign *MetaForeign, const QString &SqlFilter, const QVariant &ObjectID)
{
	PMetaTable *MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->ForeignClass); //Таблица на которую ссылается внешний ключ
	QStringList FieldList = MetaForeign->ForeignViewNameField.split(';');

	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignField.toLower() + " AS ID, concat(";

	for (int i = 0; i < FieldList.count(); ++i) //Обход полей (которые должны быть отображены)
	{
		SqlQuery += MetaTableForeign->Alias + '_' + FieldList.at(i).toLower() + ", ' ', ";
	}

	ISSystem::RemoveLastSymbolFromString(SqlQuery, 7);

	SqlQuery += ") ";
	SqlQuery += "AS Value \n";
	SqlQuery += "FROM " + MetaTableForeign->Name + " \n";
	SqlQuery += "WHERE NOT " + MetaTableForeign->Alias + "_isdeleted \n";

	if (SqlFilter.length())
	{
		SqlQuery += "AND " + SqlFilter + " \n";
	}

	if (ObjectID.isValid())
	{
		SqlQuery += "AND " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignField + " = :ObjectID \n";
	}

	SqlQuery += "ORDER BY ";

	if (!MetaForeign->OrderField.isEmpty()) //Если указано поле для сортировки
	{
		SqlQuery += MetaTableForeign->Alias + '_' + MetaForeign->OrderField + " ASC";
	}
	else
	{
		SqlQuery += "2 ASC";
	}

	return SqlQuery;
}
//-----------------------------------------------------------------------------
QString ISMetaDataHelper::GetConfigurationName()
{
	ISQuery qSelect(QS_CONFIGURATION);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("get_configuration_name").toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
