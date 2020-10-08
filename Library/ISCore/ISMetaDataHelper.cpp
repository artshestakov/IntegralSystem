#include "ISMetaDataHelper.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_DATABASE_DESCRIPTION = PREPARE_QUERY("SELECT description "
													   "FROM pg_shdescription "
													   "JOIN pg_database ON objoid = pg_database.oid "
													   "WHERE datname = current_database()");
//-----------------------------------------------------------------------------
QString ISMetaDataHelper::GenerateSqlQueryFromForeign(PMetaForeign *MetaForeign, const QString &SqlFilter, const QVariant &ObjectID)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass); //Таблица на которую ссылается внешний ключ
	QStringList FieldList = MetaForeign->ForeignViewNameField.split(';');

	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignField.toLower() + " AS ID, concat(";
	for (const QString &String : FieldList) //Обход полей (которые должны быть отображены)
	{
		SqlQuery += MetaTableForeign->Alias + '_' + String.toLower() + ", ' ', ";
	}
	SqlQuery.chop(7);
	SqlQuery += ") ";
	SqlQuery += "AS Value \n";
	SqlQuery += "FROM " + MetaTableForeign->Name + " \n";
	SqlQuery += "WHERE NOT " + MetaTableForeign->Alias + "_isdeleted \n";

	if (!SqlFilter.isEmpty())
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
QString ISMetaDataHelper::GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);
	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignViewNameField + " \n" +
		"FROM " + MetaTableForeign->Name + " \n" +
		"WHERE " + MetaTableForeign->Alias + "_id = " + Alias + '_' + FieldName;
	return SqlQuery;
}
//-----------------------------------------------------------------------------
QString ISMetaDataHelper::GetConfigurationName(QString &ErrorString)
{
	ISQuery qSelect(QS_DATABASE_DESCRIPTION);
	if (qSelect.ExecuteFirst())
	{
		QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(qSelect.ReadColumn("description").toString());
		return VariantMap["ConfigurationName"].toString();
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
