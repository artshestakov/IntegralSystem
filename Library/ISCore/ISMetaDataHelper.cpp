#include "ISMetaDataHelper.h"
#include "ISMetaData.h"
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
