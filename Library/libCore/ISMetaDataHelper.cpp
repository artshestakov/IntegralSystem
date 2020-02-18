#include "ISMetaDataHelper.h"
#include "ISMetaData.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
QString ISMetaDataHelper::GenerateSqlQueryFromForeign(PMetaClassForeign *MetaForeign, const QString &SqlFilter, const QVariant &ObjectID)
{
	PMetaClassTable *MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->ForeignClass); //������� �� ������� ��������� ������� ����
	QStringList FieldList = MetaForeign->ForeignViewNameField.split(';');

	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignField.toLower() + " AS ID, concat(";

	for (int i = 0; i < FieldList.count(); ++i) //����� ����� (������� ������ ���� ����������)
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

	if (!MetaForeign->OrderField.isEmpty()) //���� ������� ���� ��� ����������
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
