#include "ISMetaViewQuery.h"
#include "ISMetaData.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISMetaViewQuery::ISMetaViewQuery(const QString &QueryName)
	: MetaQuery(ISMetaData::GetInstanse().GetMetaQuery(QueryName)),
	OrderSort(Qt::AscendingOrder)
{
	
}
//-----------------------------------------------------------------------------
ISMetaViewQuery::~ISMetaViewQuery()
{

}
//-----------------------------------------------------------------------------
QString ISMetaViewQuery::GetQueryText()
{
	QString Result = "SELECT \n";

	for (int i = 0; i < MetaQuery->Fields.size(); ++i)
	{
		PMetaClassField *MetaQueryField = MetaQuery->Fields[i];
		Result += MetaQueryField->QueryText + " AS \"" + MetaQueryField->Name + "\", \n";
	}

	ISSystem::RemoveLastSymbolFromString(Result, 3);
	
	Result += " \n";
	Result += "FROM " + MetaQuery->Parent + " \n";

	for (int i = 0; i < MetaQuery->Joins.size(); ++i)
	{
		Result += MetaQuery->Joins[i] + " \n";
	}

	if (!MetaQuery->Where.isEmpty())
	{
		Result += "WHERE " + MetaQuery->Where + " \n";
	}

	if (!ClassFilter.isEmpty())
	{
		if (!MetaQuery->Where.isEmpty())
		{
			Result += "AND ";
		}
		else
		{
			Result += "WHERE ";
		}

		Result += ClassFilter + " \n";
	}

	if (!OrderField.isEmpty())
	{
		Result += "ORDER BY " + OrderField;

		switch (OrderSort)
		{
		case Qt::AscendingOrder: Result += " ASC"; break;
		case Qt::DescendingOrder: Result += " DESC"; break;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::SetClassFilter(const QString &class_filter)
{
	ClassFilter = class_filter;
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::ClearClassFilter()
{
	ClassFilter.clear();
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::SetOrderField(const QString &FieldName)
{
	OrderField = FieldName;
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::SetOrderSort(Qt::SortOrder order_sort)
{
	OrderSort = order_sort;
}
//-----------------------------------------------------------------------------
